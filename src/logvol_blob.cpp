#include "logvol_internal.hpp"

/* Blob callbacks */

static herr_t H5VL_log_blob_put (void *obj, const void *buf, size_t size, void *blob_id, void *ctx);
static herr_t H5VL_log_blob_get (void *obj, const void *blob_id, void *buf, size_t size, void *ctx);
static herr_t H5VL_log_blob_specific (void *obj,
									  void *blob_id,
									  H5VL_blob_specific_t specific_type,
									  va_list arguments);
static herr_t H5VL_log_blob_optional (void *obj,
									  void *blob_id,
									  H5VL_blob_optional_t opt_type,
									  va_list arguments);

const H5VL_blob_class_t H5VL_log_blob_g {
	H5VL_log_blob_put,		/* put       */
	H5VL_log_blob_get,		/* get         */
	H5VL_log_blob_specific, /* specific         */
	H5VL_log_blob_optional, /* optional         */
};

/*-------------------------------------------------------------------------
 * Function:    H5VL_log_blob_put
 *
 * Purpose:     Handles the blob 'put' callback
 *
 * Return:      SUCCEED / FAIL
 *
 *-------------------------------------------------------------------------
 */
herr_t H5VL_log_blob_put (void *obj, const void *buf, size_t size, void *blob_id, void *ctx) {
	H5VL_log_obj_t *op = (H5VL_log_obj_t *)obj;

	return H5VLblob_put (op->uo, op->uvlid, buf, size, blob_id, ctx);
} /* end H5VL_log_blob_put() */

/*-------------------------------------------------------------------------
 * Function:    H5VL_log_blob_get
 *
 * Purpose:     Handles the blob 'get' callback
 *
 * Return:      SUCCEED / FAIL
 *
 *-------------------------------------------------------------------------
 */
herr_t H5VL_log_blob_get (void *obj, const void *blob_id, void *buf, size_t size, void *ctx) {
	H5VL_log_obj_t *op = (H5VL_log_obj_t *)obj;

	return H5VLblob_get (op->uo, op->uvlid, blob_id, buf, size, ctx);
} /* end H5VL_log_blob_get() */

/*-------------------------------------------------------------------------
 * Function:    H5VL_log_blob_specific
 *
 * Purpose:     Handles the blob 'specific' callback
 *
 * Return:      SUCCEED / FAIL
 *
 *-------------------------------------------------------------------------
 */
herr_t H5VL_log_blob_specific (void *obj,
							   void *blob_id,
							   H5VL_blob_specific_t specific_type,
							   va_list arguments) {
	H5VL_log_obj_t *op = (H5VL_log_obj_t *)obj;

	return H5VLblob_specific (op->uo, op->uvlid, blob_id, specific_type,
							  arguments);
} /* end H5VL_log_blob_specific() */

/*-------------------------------------------------------------------------
 * Function:    H5VL_log_blob_optional
 *
 * Purpose:     Handles the blob 'optional' callback
 *
 * Return:      SUCCEED / FAIL
 *
 *-------------------------------------------------------------------------
 */
herr_t H5VL_log_blob_optional (void *obj,
							   void *blob_id,
							   H5VL_blob_optional_t opt_type,
							   va_list arguments) {
	H5VL_log_obj_t *op = (H5VL_log_obj_t *)obj;

	return H5VLblob_optional (op->uo, op->uvlid, blob_id, opt_type, arguments);
} /* end H5VL_log_blob_optional() */