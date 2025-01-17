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

#define H5VL_LOG_NTIMER 108

#define TIMER_H5VL_LOG_FILE_CREATE 0
#define TIMER_H5VL_LOG_FILE_CREATE_INIT 1
#define TIMER_H5VL_LOG_FILE_CREATE_FILE 2
#define TIMER_H5VL_LOG_FILE_CREATE_GROUP 3
#define TIMER_H5VL_LOG_FILE_CREATE_FH 4
#define TIMER_H5VL_LOG_FILE_CREATE_STRIPE 5
#define TIMER_H5VL_LOG_FILE_CREATE_GROUP_RANK 6
#define TIMER_H5VL_LOG_FILE_CREATE_SUBFILE 7
#define TIMER_H5VL_LOG_FILE_OPEN 8
#define TIMER_H5VL_LOG_FILE_GET 9
#define TIMER_H5VL_LOG_FILE_SPECIFIC 10
#define TIMER_H5VL_LOG_FILE_OPTIONAL 11
#define TIMER_H5VL_LOG_FILE_CLOSE 12
#define TIMER_H5VL_LOG_GROUP_CREATE 13
#define TIMER_H5VL_LOG_GROUP_OPEN 14
#define TIMER_H5VL_LOG_GROUP_GET 15
#define TIMER_H5VL_LOG_GROUP_SPECIFIC 16
#define TIMER_H5VL_LOG_GROUP_OPTIONAL 17
#define TIMER_H5VL_LOG_GROUP_CLOSE 18
#define TIMER_H5VL_LOG_ATT_CREATE 19
#define TIMER_H5VL_LOG_ATT_OPEN 20
#define TIMER_H5VL_LOG_ATT_READ 21
#define TIMER_H5VL_LOG_ATT_WRITE 22
#define TIMER_H5VL_LOG_ATT_GET 23
#define TIMER_H5VL_LOG_ATT_SPECIFIC 24
#define TIMER_H5VL_LOG_ATT_OPTIONAL 25
#define TIMER_H5VL_LOG_ATT_CLOSE 26
#define TIMER_H5VL_LOG_DATASET_CREATE 27
#define TIMER_H5VL_LOG_DATASET_OPEN 28
#define TIMER_H5VL_LOG_DATASET_READ 29
#define TIMER_H5VL_LOG_DATASET_READ_INIT 30
#define TIMER_H5VL_LOG_DATASET_WRITE 31
#define TIMER_H5VL_LOG_DATASET_WRITE_INIT 32
#define TIMER_H5VL_LOG_DATASET_WRITE_START_COUNT 33
#define TIMER_H5VL_LOG_DATASET_WRITE_ENCODE 34
#define TIMER_H5VL_LOG_DATASET_WRITE_META_DEDUP 35
#define TIMER_H5VL_LOG_DATASET_WRITE_META_ZIP 36
#define TIMER_H5VL_LOG_DATASET_WRITE_PACK 37
#define TIMER_H5VL_LOG_DATASET_WRITE_CONVERT 38
#define TIMER_H5VL_LOG_DATASET_WRITE_FILTER 39
#define TIMER_H5VL_LOG_DATASET_WRITE_FINALIZE 40
#define TIMER_H5VL_LOG_DATASET_GET 41
#define TIMER_H5VL_LOG_DATASET_SPECIFIC 42
#define TIMER_H5VL_LOG_DATASET_OPTIONAL 43
#define TIMER_H5VL_LOG_DATASET_CLOSE 44
#define TIMER_H5VLFILE_CREATE 45
#define TIMER_H5VLFILE_OPEN 46
#define TIMER_H5VLFILE_GET 47
#define TIMER_H5VLFILE_SPECIFIC 48
#define TIMER_H5VLFILE_OPTIONAL 49
#define TIMER_H5VLFILE_CLOSE 50
#define TIMER_H5VLGROUP_CREATE 51
#define TIMER_H5VLGROUP_OPEN 52
#define TIMER_H5VLGROUP_GET 53
#define TIMER_H5VLGROUP_SPECIFIC 54
#define TIMER_H5VLGROUP_OPTIONAL 55
#define TIMER_H5VLGROUP_CLOSE 56
#define TIMER_H5VLATT_CREATE 57
#define TIMER_H5VLATT_OPEN 58
#define TIMER_H5VLATT_READ 59
#define TIMER_H5VLATT_WRITE 60
#define TIMER_H5VLATT_GET 61
#define TIMER_H5VLATT_SPECIFIC 62
#define TIMER_H5VLATT_OPTIONAL 63
#define TIMER_H5VLATT_CLOSE 64
#define TIMER_H5VLDATASET_CREATE 65
#define TIMER_H5VLDATASET_OPEN 66
#define TIMER_H5VLDATASET_READ 67
#define TIMER_H5VLDATASET_WRITE 68
#define TIMER_H5VLDATASET_GET 69
#define TIMER_H5VLDATASET_SPECIFIC 70
#define TIMER_H5VLDATASET_OPTIONAL 71
#define TIMER_H5VLDATASET_CLOSE 72
#define TIMER_H5VL_LOG_FILEI_METASIZE 73
#define TIMER_H5VL_LOG_FILEI_METASIZE_RAW 74
#define TIMER_H5VL_LOG_FILEI_METASIZE_DEDUP 75
#define TIMER_H5VL_LOG_FILEI_METASIZE_ZIP 76
#define TIMER_H5VL_LOG_FILEI_FLUSH 77
#define TIMER_H5VL_LOG_FILEI_METAFLUSH 78
#define TIMER_H5VL_LOG_FILEI_METAFLUSH_INIT 79
#define TIMER_H5VL_LOG_FILEI_METAFLUSH_HASH 80
#define TIMER_H5VL_LOG_FILEI_METAFLUSH_PACK 81
#define TIMER_H5VL_LOG_FILEI_METAFLUSH_ZIP 82
#define TIMER_H5VL_LOG_FILEI_METAFLUSH_SYNC 83
#define TIMER_H5VL_LOG_FILEI_METAFLUSH_CREATE 84
#define TIMER_H5VL_LOG_FILEI_METAFLUSH_WRITE 85
#define TIMER_H5VL_LOG_FILEI_METAFLUSH_CLOSE 86
#define TIMER_H5VL_LOG_FILEI_METAFLUSH_BARRIER 87
#define TIMER_H5VL_LOG_FILEI_METAFLUSH_FINALIZE 88
#define TIMER_H5VL_LOG_FILEI_METAFLUSH_SIZE 89
#define TIMER_H5VL_LOG_FILEI_METAFLUSH_SIZE_DEDUP 90
#define TIMER_H5VL_LOG_FILEI_METAFLUSH_SIZE_ZIP 91
#define TIMER_H5VL_LOG_FILEI_METAFLUSH_REPEAT_COUNT 92
#define TIMER_H5VL_LOG_FILEI_METAUPDATE 93
#define TIMER_H5VL_LOG_DATASETI_READI_GEN_RTYPES 94
#define TIMER_H5VL_LOG_DATASETI_OPEN_WITH_UO 95
#define TIMER_H5VL_LOG_DATASETI_WRAP 96
#define TIMER_H5VL_LOGI_GET_DATASPACE_SEL_TYPE 97
#define TIMER_H5VL_LOGI_GET_DATASPACE_SELECTION 98
#define TIMER_H5VL_LOG_NB_FLUSH_READ_REQS 99
#define TIMER_H5VL_LOG_NB_FLUSH_WRITE_REQS 100
#define TIMER_H5VL_LOG_NB_FLUSH_WRITE_REQS_INIT 101
#define TIMER_H5VL_LOG_NB_FLUSH_WRITE_REQS_SYNC 102
#define TIMER_H5VL_LOG_NB_FLUSH_WRITE_REQS_CREATE 103
#define TIMER_H5VL_LOG_NB_FLUSH_WRITE_REQS_WR 104
#define TIMER_H5VL_LOG_NB_FLUSH_WRITE_REQS_CREATE_VIRTUAL 105
#define TIMER_H5VL_LOG_NB_WRITE_REQS_ALIGNED 106
#define TIMER_H5VL_LOG_NB_FLUSH_WRITE_REQS_SIZE 107
#endif

#ifdef LOGVOL_PROFILING
#include "H5VL_logi_profiling.hpp"
#define H5VL_LOGI_PROFILING_TIMER_START          \
	{                        \
		double tstart, tend; \
		tstart = MPI_Wtime ();
#define H5VL_LOGI_PROFILING_TIMER_STOP(A, B)                             \
	tend = MPI_Wtime ();                             \
	H5VL_log_profile_add_time (A, B, tend - tstart); \
	}
#else
#define H5VL_LOGI_PROFILING_TIMER_START \
	{}
#define H5VL_LOGI_PROFILING_TIMER_STOP(A, B) \
	{}
#endif

#ifdef LOGVOL_PROFILING
void H5VL_log_profile_add_time (void *file, int id, double t);
void H5VL_log_profile_sub_time (void *file, int id, double t);
void H5VL_log_profile_print (void *file);
void H5VL_log_profile_reset (void *file);
#endif