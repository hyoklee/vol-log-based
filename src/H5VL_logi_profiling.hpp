/* Do not edit this file. It is produced from the corresponding .m4 source */
/*
 *  Copyright (C) 2021, Northwestern University and Argonne National Laboratory
 *  See COPYRIGHT notice in top-level directory.
 */
/* $Id$ */




#pragma once

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

/*
 * Report performance profiling
 */
#ifdef LOGVOL_PROFILING

#define H5VL_LOG_NTIMER 87

#define TIMER_H5VL_LOG_FILE_CREATE 0
#define TIMER_H5VL_LOG_FILE_OPEN 1
#define TIMER_H5VL_LOG_FILE_GET 2
#define TIMER_H5VL_LOG_FILE_SPECIFIC 3
#define TIMER_H5VL_LOG_FILE_OPTIONAL 4
#define TIMER_H5VL_LOG_FILE_CLOSE 5
#define TIMER_H5VL_LOG_GROUP_CREATE 6
#define TIMER_H5VL_LOG_GROUP_OPEN 7
#define TIMER_H5VL_LOG_GROUP_GET 8
#define TIMER_H5VL_LOG_GROUP_SPECIFIC 9
#define TIMER_H5VL_LOG_GROUP_OPTIONAL 10
#define TIMER_H5VL_LOG_GROUP_CLOSE 11
#define TIMER_H5VL_LOG_ATT_CREATE 12
#define TIMER_H5VL_LOG_ATT_OPEN 13
#define TIMER_H5VL_LOG_ATT_READ 14
#define TIMER_H5VL_LOG_ATT_WRITE 15
#define TIMER_H5VL_LOG_ATT_GET 16
#define TIMER_H5VL_LOG_ATT_SPECIFIC 17
#define TIMER_H5VL_LOG_ATT_OPTIONAL 18
#define TIMER_H5VL_LOG_ATT_CLOSE 19
#define TIMER_H5VL_LOG_DATASET_CREATE 20
#define TIMER_H5VL_LOG_DATASET_OPEN 21
#define TIMER_H5VL_LOG_DATASET_READ 22
#define TIMER_H5VL_LOG_DATASET_WRITE 23
#define TIMER_H5VL_LOG_DATASET_WRITE_INIT 24
#define TIMER_H5VL_LOG_DATASET_WRITE_START_COUNT 25
#define TIMER_H5VL_LOG_DATASET_WRITE_ENCODE 26
#define TIMER_H5VL_LOG_DATASET_WRITE_META_DEFLATE 27
#define TIMER_H5VL_LOG_DATASET_WRITE_PACK 28
#define TIMER_H5VL_LOG_DATASET_WRITE_CONVERT 29
#define TIMER_H5VL_LOG_DATASET_WRITE_FILTER 30
#define TIMER_H5VL_LOG_DATASET_WRITE_FINALIZE 31
#define TIMER_H5VL_LOG_DATASET_GET 32
#define TIMER_H5VL_LOG_DATASET_SPECIFIC 33
#define TIMER_H5VL_LOG_DATASET_OPTIONAL 34
#define TIMER_H5VL_LOG_DATASET_CLOSE 35
#define TIMER_H5VLFILE_CREATE 36
#define TIMER_H5VLFILE_OPEN 37
#define TIMER_H5VLFILE_GET 38
#define TIMER_H5VLFILE_SPECIFIC 39
#define TIMER_H5VLFILE_OPTIONAL 40
#define TIMER_H5VLFILE_CLOSE 41
#define TIMER_H5VLGROUP_CREATE 42
#define TIMER_H5VLGROUP_OPEN 43
#define TIMER_H5VLGROUP_GET 44
#define TIMER_H5VLGROUP_SPECIFIC 45
#define TIMER_H5VLGROUP_OPTIONAL 46
#define TIMER_H5VLGROUP_CLOSE 47
#define TIMER_H5VLATT_CREATE 48
#define TIMER_H5VLATT_OPEN 49
#define TIMER_H5VLATT_READ 50
#define TIMER_H5VLATT_WRITE 51
#define TIMER_H5VLATT_GET 52
#define TIMER_H5VLATT_SPECIFIC 53
#define TIMER_H5VLATT_OPTIONAL 54
#define TIMER_H5VLATT_CLOSE 55
#define TIMER_H5VLDATASET_CREATE 56
#define TIMER_H5VLDATASET_OPEN 57
#define TIMER_H5VLDATASET_READ 58
#define TIMER_H5VLDATASET_WRITE 59
#define TIMER_H5VLDATASET_GET 60
#define TIMER_H5VLDATASET_SPECIFIC 61
#define TIMER_H5VLDATASET_OPTIONAL 62
#define TIMER_H5VLDATASET_CLOSE 63
#define TIMER_H5VL_LOG_FILEI_FLUSH 64
#define TIMER_H5VL_LOG_FILEI_METAFLUSH 65
#define TIMER_H5VL_LOG_FILEI_METAFLUSH_INIT 66
#define TIMER_H5VL_LOG_FILEI_METAFLUSH_PACK 67
#define TIMER_H5VL_LOG_FILEI_METAFLUSH_ZIP 68
#define TIMER_H5VL_LOG_FILEI_METAFLUSH_SYNC 69
#define TIMER_H5VL_LOG_FILEI_METAFLUSH_CREATE 70
#define TIMER_H5VL_LOG_FILEI_METAFLUSH_WRITE 71
#define TIMER_H5VL_LOG_FILEI_METAFLUSH_CLOSE 72
#define TIMER_H5VL_LOG_FILEI_METAFLUSH_BARRIER 73
#define TIMER_H5VL_LOG_FILEI_METAFLUSH_FINALIZE 74
#define TIMER_H5VL_LOG_FILEI_METAFLUSH_SIZE 75
#define TIMER_H5VL_LOG_FILEI_METAFLUSH_SIZE_ZIP 76
#define TIMER_H5VL_LOG_FILEI_METAUPDATE 77
#define TIMER_H5VL_LOG_DATASETI_READI_GEN_RTYPES 78
#define TIMER_H5VL_LOG_DATASETI_OPEN_WITH_UO 79
#define TIMER_H5VL_LOG_DATASETI_WRAP 80
#define TIMER_H5VL_LOGI_GET_DATASPACE_SEL_TYPE 81
#define TIMER_H5VL_LOGI_GET_DATASPACE_SELECTION 82
#define TIMER_H5VL_LOG_NB_FLUSH_READ_REQS 83
#define TIMER_H5VL_LOG_NB_FLUSH_WRITE_REQS 84
#define TIMER_H5VL_LOG_NB_WRITE_REQS_ALIGNED 85
#define TIMER_H5VL_LOG_NB_FLUSH_WRITE_REQS_SIZE 86
#endif

#ifdef LOGVOL_PROFILING
#include "H5VL_logi_profiling.hpp"
#define TIMER_H5VL_LOG_START          \
	{                        \
		double tstart, tend; \
		tstart = MPI_Wtime ();
#define TIMER_H5VL_LOG_STOP(A, B)                             \
	tend = MPI_Wtime ();                             \
	H5VL_log_profile_add_time (A, B, tend - tstart); \
	}
#else
#define TIMER_H5VL_LOG_START \
	{}
#define TIMER_H5VL_LOG_STOP(A, B) \
	{}
#endif

#ifdef LOGVOL_PROFILING
void H5VL_log_profile_add_time (void *file, int id, double t);
void H5VL_log_profile_sub_time (void *file, int id, double t);
void H5VL_log_profile_print (void *file);
void H5VL_log_profile_reset (void *file);
#endif