#include "logvol_internal.hpp"

/********************* */
/* Function prototypes */
/********************* */
void *H5VL_log_file_create(const char *name, unsigned flags, hid_t fcpl_id, hid_t fapl_id, hid_t dxpl_id, void **req);
void *H5VL_log_file_open(const char *name, unsigned flags, hid_t fapl_id, hid_t dxpl_id, void **req);
herr_t H5VL_log_file_get(void *file, H5VL_file_get_t get_type, hid_t dxpl_id, void **req, va_list arguments);
herr_t H5VL_log_file_specific(void *file, H5VL_file_specific_t specific_type, hid_t dxpl_id, void **req, va_list arguments);
herr_t H5VL_log_file_optional(void *file, H5VL_file_optional_t opt_type, hid_t dxpl_id, void **req, va_list arguments);
herr_t H5VL_log_file_close(void *file, hid_t dxpl_id, void **req);

const H5VL_file_class_t H5VL_log_file_g{
    H5VL_log_file_create,                       /* create */
    H5VL_log_file_open,                         /* open */
    H5VL_log_file_get,                          /* get */
    H5VL_log_file_specific,                     /* specific */
    H5VL_log_file_optional,                     /* optional */
    H5VL_log_file_close                         /* close */
};

/*-------------------------------------------------------------------------
 * Function:    H5VL_log_file_create
 *
 * Purpose:     Creates a container using this connector
 *
 * Return:      Success:    Pointer to a file object
 *              Failure:    NULL
 *
 *-------------------------------------------------------------------------
 */
void *H5VL_log_file_create(const char *name, unsigned flags, hid_t fcpl_id,
                            hid_t fapl_id, hid_t dxpl_id, void **req) {
    herr_t err;
    int mpierr;
    H5VL_log_info_t *info = NULL;
    H5VL_log_file_t *fp = NULL;
    H5VL_loc_params_t loc;
    hid_t uvlid, under_fapl_id;
    void *under_vol_info;
    MPI_Comm comm;
    hbool_t po_supported;

    // Try get info about under VOL
    H5Pget_vol_info(fapl_id, (void **)&info);

    if (info){
        uvlid = info->uvlid;
        under_vol_info = info->under_vol_info;
    }
    else{   // If no under VOL specified, use the native VOL
        assert(H5VLis_connector_registered_by_name("native") == 1);
        uvlid = H5VLget_connector_id_by_name("native");
        assert(uvlid > 0);
        under_vol_info = NULL;
        //return NULL;
    }

    // Make sure we have mpi enabled
    err = H5Pget_fapl_mpio(fapl_id, &comm, NULL); CHECK_ERR

    // Init file obj
    fp = new H5VL_log_file_t();
    fp->closing = false;
    fp->refcnt = 0;
    fp->nldset = 0;
    fp->ndset = 0;
    MPI_Comm_dup(comm, &(fp->comm));
    MPI_Comm_rank(comm, &(fp->rank));
    fp->uvlid = uvlid;
    fp->nflushed = 0;
    fp->dxplid = dxpl_id;
    fp->type = H5I_FILE;
    fp->idxvalid = false;
    fp->metadirty = false;

    // Create the file with underlying VOL
    under_fapl_id = H5Pcopy(fapl_id);
    H5Pset_vol(under_fapl_id, uvlid, under_vol_info);
    fp->uo = H5VLfile_create(name, flags, fcpl_id, under_fapl_id, dxpl_id, NULL); CHECK_NERR(fp->uo)
    H5Pclose(under_fapl_id);
    /* Make the 'post open' callback */
    // A workaround without introspect support
    po_supported = 0;
    //err = H5VLintrospect_opt_query(fp->uo, fp->uvlid, H5VL_SUBCLS_FILE, H5VL_NATIVE_FILE_POST_OPEN, &po_supported); CHECK_ERR
    if(po_supported){
        //err = H5VLfile_optional_wrapper(fp->uo, fp->uvlid, H5VL_NATIVE_FILE_POST_OPEN, H5P_DATASET_XFER_DEFAULT, NULL); CHECK_ERR
    }
    
    
    // Create LOG group
    loc.obj_type = H5I_FILE;
    loc.type = H5VL_OBJECT_BY_SELF;
    fp->lgp = H5VLgroup_create(fp->uo, &loc, fp->uvlid, LOG_GROUP_NAME, H5P_LINK_CREATE_DEFAULT, H5P_GROUP_CREATE_DEFAULT,  H5P_DEFAULT, dxpl_id, NULL); CHECK_NERR(fp->lgp)

    // Att
    err = H5VL_logi_add_att(fp, "_ndset", H5T_STD_I32LE, H5T_NATIVE_INT32, 1, &(fp->ndset), dxpl_id); CHECK_ERR
    err = H5VL_logi_add_att(fp, "_nldset", H5T_STD_I32LE, H5T_NATIVE_INT32, 1, &(fp->nldset), dxpl_id); CHECK_ERR

    // Open the file with MPI
    mpierr = MPI_File_open(fp->comm, name, MPI_MODE_RDWR, MPI_INFO_NULL, &(fp->fh)); CHECK_MPIERR

    goto fn_exit;
err_out:;
    if (fp){
        delete fp;
    }
    fp = NULL;
fn_exit:;
    MPI_Comm_free(&comm);

    if (info){
        free(info);
    }

    return (void *)fp;
} /* end H5VL_log_file_create() */


/*-------------------------------------------------------------------------
 * Function:    H5VL_log_file_open
 *
 * Purpose:     Opens a container created with this connector
 *
 * Return:      Success:    Pointer to a file object
 *              Failure:    NULL
 *
 *-------------------------------------------------------------------------
 */
void *H5VL_log_file_open(const char *name, unsigned flags, hid_t fapl_id,
                            hid_t dxpl_id, void **req){
    herr_t err;
    int mpierr;
    H5VL_log_info_t *info = NULL;
    H5VL_log_file_t *fp = NULL;
    H5VL_loc_params_t loc;
    hid_t uvlid, under_fapl_id;
    void *under_vol_info;
    MPI_Comm comm;

    // Try get info about under VOL
    H5Pget_vol_info(fapl_id, (void **)&info);

    if (info){
        uvlid = info->uvlid;
        under_vol_info = info->under_vol_info;
    }
    else{   // If no under VOL specified, use the native VOL
        assert(H5VLis_connector_registered_by_name("native") == 1);
        uvlid = H5VLget_connector_id_by_name("native");
        assert(uvlid > 0);
        under_vol_info = NULL;
        //return NULL;
    }

    // Make sure we have mpi enabled
    err = H5Pget_fapl_mpio(fapl_id, &comm, NULL); CHECK_ERR

    // Init file obj
    fp = new H5VL_log_file_t();
    fp->closing = false;
    fp->refcnt = 0;
    MPI_Comm_dup(comm, &(fp->comm));
    MPI_Comm_rank(comm, &(fp->rank));
    fp->uvlid = uvlid;
    fp->nflushed = 0;
    fp->dxplid = dxpl_id;
    fp->type = H5I_FILE;
    fp->idxvalid = false;
    fp->metadirty = false;

    // Create the file with underlying VOL
    under_fapl_id = H5Pcopy(fapl_id);
    H5Pset_vol(under_fapl_id, uvlid, under_vol_info);
    fp->uo = H5VLfile_open(name, flags, under_fapl_id, dxpl_id, NULL); CHECK_NERR(fp->uo)
    H5Pclose(under_fapl_id);
    
    // Create LOG group
    loc.obj_type = H5I_FILE;
    loc.type = H5VL_OBJECT_BY_SELF;
    fp->lgp = H5VLgroup_open(fp->uo, &loc, fp->uvlid, LOG_GROUP_NAME, H5P_DEFAULT, dxpl_id, NULL); CHECK_NERR(fp->lgp)

    // Att
    err = H5VL_logi_get_att(fp, "_ndset", H5T_NATIVE_INT32, &(fp->ndset), dxpl_id); CHECK_ERR
    err = H5VL_logi_get_att(fp, "_nldset", H5T_NATIVE_INT32, &(fp->nldset), dxpl_id); CHECK_ERR
    fp->idx.resize(fp->ndset);

    // Open the file with MPI
    mpierr = MPI_File_open(fp->comm, name, MPI_MODE_RDWR, MPI_INFO_NULL, &(fp->fh)); CHECK_MPIERR

    goto fn_exit;
err_out:;
    if (fp){
        delete fp;
    }
    fp = NULL;
fn_exit:;
    MPI_Comm_free(&comm);

    if (info){
        free(info);
    }

    return (void *)fp;
} /* end H5VL_log_file_open() */


/*-------------------------------------------------------------------------
 * Function:    H5VL_log_file_get
 *
 * Purpose:     Get info about a file
 *
 * Return:      Success:    0
 *              Failure:    -1
 *
 *-------------------------------------------------------------------------
 */
herr_t H5VL_log_file_get(void *file, H5VL_file_get_t get_type, hid_t dxpl_id,
                            void **req, va_list arguments){
    herr_t err;
    H5VL_log_file_t *fp = (H5VL_log_file_t*)file;
    
#ifdef ENABLE_PASSTHRU_LOGGING
    printf("------- LOG VOL FILE Get\n");
#endif

    err = H5VLfile_get(fp->uo, fp->uvlid, get_type, dxpl_id, req, arguments); CHECK_ERR

err_out:;
    return err;
} /* end H5VL_log_file_get() */

/*-------------------------------------------------------------------------
 * Function:    H5VL_log_file_specific
 *
 * Purpose:     Specific operation on file
 *
 * Return:      Success:    0
 *              Failure:    -1
 *
 *-------------------------------------------------------------------------
 */
herr_t H5VL_log_file_specific(  void *file, H5VL_file_specific_t specific_type,
                                hid_t dxpl_id, void **req, va_list arguments) {
    herr_t err;
    H5VL_log_file_t *fp = (H5VL_log_file_t*)file;
    
    switch(specific_type) {
        case H5VL_FILE_IS_ACCESSIBLE:
        case H5VL_FILE_DELETE:
            {
                hid_t uvlid, under_fapl_id, fapl_id;
                void *under_vol_info;
                H5VL_log_info_t *info = NULL;

                // Try get info about under VOL
                fapl_id = va_arg(arguments, hid_t);
                H5Pget_vol_info(fapl_id, (void **)&info);
                if (info){
                    uvlid = info->uvlid;
                    under_vol_info = info->under_vol_info;
                    free(info);
                }
                else{   // If no under VOL specified, use the native VOL
                    //assert(H5VLis_connector_registered("native") == 1);
                    //uvlid = H5VLget_connector_id_by_name("native");
                    //assert(uvlid > 0);
                    //under_vol_info = NULL;
                }

                /* Call specific of under VOL */
                under_fapl_id = H5Pcopy(fapl_id);
                H5Pset_vol(under_fapl_id, uvlid, under_vol_info);
                err = H5VLfile_specific(NULL, uvlid, specific_type, dxpl_id, req, arguments); CHECK_ERR
                H5Pclose(under_fapl_id);
            }
            break;
        case H5VL_FILE_FLUSH:
            {
                err = H5VL_logi_file_flush(fp, dxpl_id);
                break;
            }
            break;
        default:
            RET_ERR("Unsupported specific_type")
    } /* end select */

err_out:;
    return err;
} /* end H5VL_log_file_specific() */


/*-------------------------------------------------------------------------
 * Function:    H5VL_log_file_optional
 *
 * Purpose:     Perform a connector-specific operation on a file
 *
 * Return:      Success:    0
 *              Failure:    -1
 *
 *-------------------------------------------------------------------------
 */
herr_t H5VL_log_file_optional(void *file, H5VL_file_optional_t opt_type, hid_t dxpl_id, void **req, va_list arguments) {
    herr_t err;
    H5VL_log_file_t *fp = (H5VL_log_file_t*)file;

#ifdef ENABLE_PASSTHRU_LOGGING
    printf("------- LOG VOL File Optional\n");
#endif

    err = H5VLfile_optional(fp->uo, fp->uvlid, opt_type, dxpl_id, req, arguments); CHECK_ERR

err_out:;
    return err;
} /* end H5VL_log_file_optional() */


/*-------------------------------------------------------------------------
 * Function:    H5VL_log_file_close
 *
 * Purpose:     Closes a file.
 *
 * Return:      Success:    0
 *              Failure:    -1, file not closed.
 *
 *-------------------------------------------------------------------------
 */
herr_t H5VL_log_file_close(void *file, hid_t dxpl_id, void **req) {
    herr_t err = 0;
    int mpierr;
    H5VL_log_file_t *fp = (H5VL_log_file_t*)file;

#ifdef ENABLE_PASSTHRU_LOGGING
    printf("------- LOG VOL FILE Close\n");
#endif

    // Flush the file
    err = H5VL_logi_file_flush(fp, dxpl_id); CHECK_ERR

    // Generate metadata table
    err = H5VL_logi_file_metaflush(fp); CHECK_ERR

    // Att
    err = H5VL_logi_get_att(fp, "_ndset", H5T_NATIVE_INT32, &(fp->ndset), dxpl_id); CHECK_ERR

    // Close log group
    err = H5VLgroup_close(fp->lgp, fp->uvlid, dxpl_id, req); CHECK_ERR

    // Close the file with MPI
    mpierr = MPI_File_close(&(fp->fh)); CHECK_MPIERR

    // Close the file with under VOL
    err = H5VLfile_close(fp->uo, fp->uvlid, dxpl_id, req); CHECK_ERR

    // Clean up
    MPI_Comm_free(&(fp->comm));
    delete fp;

err_out:
    return err;
} /* end H5VL_log_file_close() */

herr_t H5VL_logi_file_flush(H5VL_log_file_t *fp, hid_t dxplid){
    herr_t err;
    int mpierr;
    int i;
    int cnt;
    int *mlens = NULL;
    MPI_Aint *moffs = NULL;
    MPI_Datatype mtype = MPI_DATATYPE_NULL;
    MPI_Status stat;
    MPI_Offset fsize_local, fsize_all, foff;
    void *ldp;
    hid_t ldsid = -1;
    hsize_t dsize;
    haddr_t doff;
    H5VL_loc_params_t loc;
    char dname[16];

    cnt = fp->wreqs.size() - fp->nflushed;

    // Construct memory type
    mlens = new int[cnt];
    moffs = new MPI_Aint[cnt];
    fsize_local = 0;
    for(i = fp->nflushed; i < fp->wreqs.size(); i++){
        moffs[i - fp->nflushed] = (MPI_Aint)fp->wreqs[i].buf;
        mlens[i - fp->nflushed] = fp->wreqs[i].rsize;
        fp->wreqs[i].ldoff = fsize_local;
        fsize_local += fp->wreqs[i].rsize;
    }
    mpierr = MPI_Type_hindexed(cnt, mlens, moffs, MPI_BYTE, &mtype); CHECK_MPIERR
    mpierr = MPI_Type_commit(&mtype); CHECK_MPIERR

    // Get file offset and total size
    mpierr = MPI_Allreduce(&fsize_local, &fsize_all, 1, MPI_LONG_LONG, MPI_SUM, fp->comm); CHECK_MPIERR
    // NOTE: Some MPI implementation do not produce output for rank 0, foff must ne initialized to 0
    foff = 0;
    mpierr = MPI_Exscan(&fsize_local, &foff, 1, MPI_LONG_LONG, MPI_SUM, fp->comm); CHECK_MPIERR

    // Create log dataset
    dsize = (hsize_t)fsize_all;
    ldsid = H5Screate_simple(1, &dsize, &dsize); CHECK_ID(ldsid)
    sprintf(dname, "_ld_%d", fp->nldset);
    loc.obj_type = H5I_GROUP;
    loc.type = H5VL_OBJECT_BY_SELF;
    ldp = H5VLdataset_create(fp->lgp, &loc, fp->uvlid, dname, H5P_LINK_CREATE_DEFAULT, H5T_STD_B8LE, ldsid, H5P_DATASET_CREATE_DEFAULT, H5P_DATASET_ACCESS_DEFAULT, dxplid, NULL); CHECK_NERR(ldp);
    err = H5VLdataset_optional_wrapper(ldp, fp->uvlid, H5VL_NATIVE_DATASET_GET_OFFSET, dxplid, NULL, &doff); CHECK_ERR // Get dataset file offset
    err = H5VLdataset_close(ldp, fp->uvlid, dxplid, NULL); CHECK_ERR // Close the dataset

    // Write the data
    foff += doff;
    mpierr = MPI_File_write_at_all(fp->fh, foff, MPI_BOTTOM, 1, mtype, &stat); CHECK_MPIERR

    // Update metadata
    for(i = fp->nflushed; i < fp->wreqs.size(); i++){
        fp->wreqs[i].ldid = fp->nldset;
        fp->wreqs[i].ldoff += foff;
        if (fp->wreqs[i].buf_alloc){
            delete fp->wreqs[i].buf;
        }
    }
    (fp->nldset)++;
    fp->nflushed = fp->wreqs.size();

    if (fsize_all){
        fp->metadirty = true;
    }
    
err_out:
    // Cleanup
    if (mtype != MPI_DATATYPE_NULL) MPI_Type_free(&mtype);
    if (ldsid != -1) H5Sclose(ldsid);    

    return err;
}

herr_t H5VL_logi_file_metaflush(H5VL_log_file_t *fp){
    herr_t err;
    int mpierr;
    int i;
    int cnt;
    MPI_Offset *mlens = NULL, *mlens_all;
    MPI_Offset *moffs = NULL, *doffs;
    char *buf = NULL;
    char **bufp = NULL;
    H5VL_loc_params_t loc;
    void *mdp, *ldp;
    hid_t mdsid = -1, ldsid = -1, mmsid = -1, lmsid = -1;
    hsize_t start, count;
    hsize_t dsize, msize;

    // Calculate size and offset of the metadata per dataset
    mlens = new MPI_Offset[fp->ndset * 2];
    mlens_all = mlens + fp->ndset;
    moffs = new MPI_Offset[(fp->ndset + 1) * 2];
    doffs = moffs + fp->ndset + 1;
    memset(mlens, 0, sizeof(MPI_Offset) * fp->ndset);
    for(auto &rp : fp->wreqs){
        mlens[rp.did] += rp.ndim * sizeof(MPI_Offset) * 2 + sizeof(int) + sizeof(MPI_Offset) + sizeof(size_t);
    }
    moffs[0] = 0;
    for(i = 0; i < fp->ndset; i++){
        moffs[i + 1] = moffs[i] + mlens[i];
    }

    // Pack data
    buf = new char[moffs[fp->ndset]];
    bufp = new char*[fp->ndset];
    
    for(i = 0; i < fp->ndset; i++){
        bufp[i] = buf + moffs[i];
    }
    for(auto &rp : fp->wreqs){
        *((int*)bufp[rp.did]) = rp.did;
        bufp[rp.did] += sizeof(int);
        memcpy(bufp[rp.did], rp.start, rp.ndim * sizeof(MPI_Offset));
        bufp[rp.did] += rp.ndim * sizeof(MPI_Offset);
        memcpy(bufp[rp.did], rp.count, rp.ndim * sizeof(MPI_Offset));
        bufp[rp.did] += rp.ndim * sizeof(MPI_Offset);
        *((MPI_Offset*)bufp[rp.did]) = rp.ldoff;
        bufp[rp.did] += sizeof(MPI_Offset);
        *((size_t*)bufp[rp.did]) = rp.rsize;
        bufp[rp.did] += sizeof(size_t);
    }

    // Sync metadata size
    mpierr = MPI_Allreduce(mlens, mlens_all, fp->ndset, MPI_LONG_LONG, MPI_SUM, fp->comm); CHECK_MPIERR
    // NOTE: Some MPI implementation do not produce output for rank 0, moffs must ne initialized to 0
    memset(moffs, 0, sizeof(MPI_Offset) * fp->ndset);
    mpierr = MPI_Exscan(mlens, moffs, fp->ndset, MPI_LONG_LONG, MPI_SUM, fp->comm); CHECK_MPIERR
    doffs[0] = 0;
    for(i = 0; i < fp->ndset; i++){
        doffs[i + 1] = doffs[i] + mlens_all[i];
        moffs[i] += doffs[i];
    }

    // Create metadata dataset
    loc.obj_type = H5I_GROUP;
    loc.type = H5VL_OBJECT_BY_SELF;
    mdp = H5VLdataset_open(fp->lgp, &loc, fp->uvlid, "_idx", H5P_DATASET_ACCESS_DEFAULT, fp->dxplid, NULL);
    if (mdp){
        // Look up table must be created at the same time
        ldp = H5VLdataset_open(fp->lgp, &loc, fp->uvlid, "_lookup", H5P_DATASET_ACCESS_DEFAULT, fp->dxplid, NULL); CHECK_NERR(ldp)

        // Resize both dataset
        dsize = (hsize_t)doffs[fp->ndset];
        err = H5VLdataset_specific_wrapper(mdp, fp->uvlid, H5VL_DATASET_SET_EXTENT, fp->dxplid, NULL, &dsize);
        dsize = fp->ndset;
        err = H5VLdataset_specific_wrapper(ldp, fp->uvlid, H5VL_DATASET_SET_EXTENT, fp->dxplid, NULL, &dsize);

        // Get data space
        err = H5VLdataset_get_wrapper(mdp, fp->uvlid, H5VL_DATASET_GET_SPACE, fp->dxplid, NULL, &mdsid); CHECK_ERR
        err = H5VLdataset_get_wrapper(ldp, fp->uvlid, H5VL_DATASET_GET_SPACE, fp->dxplid, NULL, &ldsid); CHECK_ERR
    }
    else {  // Create the idx dataset and look up table dataset
        hid_t mdcplid, ldcplid;
        
        // Space
        dsize = (hsize_t)doffs[fp->ndset];
        msize = H5S_UNLIMITED;
        mdsid = H5Screate_simple(1, &dsize, &msize); CHECK_ID(mdsid)
        dsize = fp->ndset;
        msize = H5S_UNLIMITED;
        ldsid = H5Screate_simple(1, &dsize, &msize); CHECK_ID(ldsid)

        // Chunk
        mdcplid = H5Pcreate(H5P_DATASET_CREATE); CHECK_ID(mdcplid)
        dsize = 1048576;
        err = H5Pset_chunk(mdcplid, 1, &dsize); CHECK_ERR
        ldcplid = H5Pcreate(H5P_DATASET_CREATE); CHECK_ID(ldcplid)
        dsize = 128;
        err = H5Pset_chunk(ldcplid, 1, &dsize); CHECK_ERR

        // Create
        mdp = H5VLdataset_create(fp->lgp, &loc, fp->uvlid, "_idx", H5P_LINK_CREATE_DEFAULT, H5T_STD_B8LE, mdsid, mdcplid, H5P_DATASET_ACCESS_DEFAULT, fp->dxplid, NULL); CHECK_NERR(mdp);
        ldp = H5VLdataset_create(fp->lgp, &loc, fp->uvlid, "_lookup", H5P_LINK_CREATE_DEFAULT, H5T_STD_I64LE, ldsid, ldcplid, H5P_DATASET_ACCESS_DEFAULT, fp->dxplid, NULL); CHECK_NERR(ldp);
    }

    // Write metadata
    err = H5Sselect_none(mdsid); CHECK_ERR
    for(i = 0; i < fp->ndset; i++){
        start = moffs[i];
        count = mlens[i];
        err = H5Sselect_hyperslab(mdsid, H5S_SELECT_OR, &start, NULL, &count, NULL); CHECK_ERR
    }
    msize = moffs[fp->ndset];
    mmsid = H5Screate_simple(1, &msize, &msize); CHECK_ID(mmsid)
    msize = fp->ndset;
    lmsid = H5Screate_simple(1, &msize, &msize); CHECK_ID(lmsid)
    if (fp->rank == 0){
        start = 0;
        count = fp->ndset;
        err = H5Sselect_hyperslab(ldsid, H5S_SELECT_SET, &start, NULL, &count, NULL); CHECK_ERR
    }
    else{
        err = H5Sselect_none(ldsid); CHECK_ERR
        err = H5Sselect_none(lmsid); CHECK_ERR
    }
    err = H5VLdataset_write(mdp, fp->uvlid, H5T_NATIVE_B8, mmsid, mdsid, fp->dxplid, buf, NULL);    CHECK_ERR
    err = H5VLdataset_write(ldp, fp->uvlid, H5T_STD_I64LE, lmsid, ldsid, fp->dxplid, doffs, NULL);    CHECK_ERR

    // Close the dataset
    err = H5VLdataset_close(mdp, fp->uvlid, fp->dxplid, NULL); CHECK_ERR 
    err = H5VLdataset_close(ldp, fp->uvlid, fp->dxplid, NULL); CHECK_ERR 

    if (moffs[fp->ndset] > 0){
        fp->idxvalid = false;
    }

    fp->metadirty = false;

err_out:
    // Cleanup
    if (mlens != NULL) delete mlens;
    if (moffs != NULL) delete moffs;
    if (buf != NULL) delete buf;
    if (bufp != NULL) delete bufp;
    if (mdsid >= 0) H5Sclose(mdsid);
    if (ldsid >= 0) H5Sclose(ldsid);
    if (mmsid >= 0) H5Sclose(mmsid);
    if (lmsid >= 0) H5Sclose(lmsid);

    return err;
}

herr_t H5VL_logi_file_metaupdate(H5VL_log_file_t *fp){
    herr_t err;
    int i;
    H5VL_loc_params_t loc;
    htri_t mdexist;
    void *mdp = NULL, *ldp = NULL;
    hid_t mdsid = -1, ldsid = -1;
    hsize_t mdsize, ldsize;
    char *buf = NULL, *bufp;
    int ndim;
    H5VL_log_metaentry_t entry;

    if (fp->metadirty){
        H5VL_logi_file_metaflush(fp);
    }

    loc.obj_type = H5I_GROUP;
    loc.type = H5VL_OBJECT_BY_SELF;
    loc.loc_data.loc_by_name.name = "_idx";
    loc.loc_data.loc_by_name.lapl_id = H5P_LINK_ACCESS_DEFAULT;
    err = H5VLlink_specific_wrapper(fp->lgp, &loc, fp->uvlid, H5VL_LINK_EXISTS, fp->dxplid, NULL, &mdexist); CHECK_ERR

    if (mdexist > 0){
        mdp = H5VLdataset_open(fp->lgp, &loc, fp->uvlid, "_idx", H5P_DATASET_ACCESS_DEFAULT, fp->dxplid, NULL); CHECK_NERR(mdp)
        //ldp = H5VLdataset_open(fp->lgp, &loc, fp->uvlid, "_lookup", H5P_DATASET_ACCESS_DEFAULT, fp->dxplid, NULL); CHECK_NERR(ldp)

        // Get data space and size
        err = H5VLdataset_get_wrapper(mdp, fp->uvlid, H5VL_DATASET_GET_SPACE, fp->dxplid, NULL, &mdsid); CHECK_ERR
        //err = H5VLdataset_get_wrapper(ldp, fp->uvlid, H5VL_DATASET_GET_SPACE, fp->dxplid, NULL, &ldsid); CHECK_ERR
        ndim = H5Sget_simple_extent_dims(mdsid, &mdsize, NULL); assert(ndim == 1);
        //ndim = H5Sget_simple_extent_dims(ldsid, &ldsize, NULL); assert(ndim == 1);

        // Allocate buffer
        buf = new char[mdsize];
        //fp->lut.resize(ldsize);

        // Read metadata
        err = H5VLdataset_read(mdp, fp->uvlid, H5T_NATIVE_B8, mdsid, mdsid, fp->dxplid, buf, NULL);    CHECK_ERR
        //err = H5VLdataset_read(ldp, fp->uvlid, H5T_STD_I64LE, ldsid, ldsid, fp->dxplid, fp->lut.data(), NULL);    CHECK_ERR

        // Close the dataset
        err = H5VLdataset_close(mdp, fp->uvlid, fp->dxplid, NULL); CHECK_ERR 
        //err = H5VLdataset_close(ldp, fp->uvlid, fp->dxplid, NULL); CHECK_ERR 

        // Parse metadata
        bufp = buf;
        while(bufp < buf + mdsize) {
            entry.did = *((int*)bufp);
            bufp += sizeof(int);
            memcpy(entry.start, bufp, fp->ndim[entry.did] * sizeof(MPI_Offset));
            bufp += fp->ndim[entry.did] * sizeof(MPI_Offset);
            memcpy(entry.count, bufp, fp->ndim[entry.did] * sizeof(MPI_Offset));
            bufp += fp->ndim[entry.did] * sizeof(MPI_Offset);
            entry.ldoff = *((MPI_Offset*)bufp);
            bufp += sizeof(MPI_Offset);
            entry.rsize = *((size_t*)bufp);
            bufp += sizeof(size_t);

            fp->idx[entry.did].push_back(entry);
        }
    }
    else{
        for(i = 0; i < fp->ndset; i++){
            fp->idx[i].clear();
        }
    }
    
    fp->idxvalid = true;

err_out:;

    // Cleanup
    if (mdsid >= 0) H5Sclose(mdsid);
    if (ldsid >= 0) H5Sclose(ldsid);
    if (buf != NULL) delete buf;

    return err;
}