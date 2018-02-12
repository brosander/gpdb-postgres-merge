/*
 *	relfilenode.c
 *
 *	relfilenode functions
 *
 *	Copyright (c) 2010, PostgreSQL Global Development Group
 *	$PostgreSQL: pgsql/contrib/pg_upgrade/relfilenode.c,v 1.8 2010/07/06 19:18:55 momjian Exp $
 */

#include "pg_upgrade.h"

#include "catalog/pg_class.h"
#include "access/transam.h"


static void transfer_single_new_db(migratorContext *ctx, pageCnvCtx *pageConverter,
					   FileNameMap *maps, int size);
static void transfer_relfile(migratorContext *ctx, pageCnvCtx *pageConverter,
				 const char *fromfile, const char *tofile,
				 const char *oldnspname, const char *oldrelname,
<<<<<<< HEAD
				 const char *newnspname, const char *newrelname,
				 bool gpdb4_heap_conversion_needed,
				 bool has_numerics, AttInfo *atts, int natts, RelType reltype);
=======
				 const char *newnspname, const char *newrelname);
>>>>>>> 1084f317702e1a039696ab8a37caf900e55ec8f2

/* used by scandir(), must be global */
char		scandir_file_pattern[MAXPGPATH];

/*
 * transfer_all_new_dbs()
 *
 * Responsible for upgrading all database. invokes routines to generate mappings and then
 * physically link the databases.
 */
const char *
transfer_all_new_dbs(migratorContext *ctx, DbInfoArr *olddb_arr,
					 DbInfoArr *newdb_arr, char *old_pgdata, char *new_pgdata)
{
	int			dbnum;
	const char *msg = NULL;

<<<<<<< HEAD
	prep_status(ctx, "Restoring user relation files");
=======
	prep_status(ctx, "Restoring user relation files\n");
>>>>>>> 1084f317702e1a039696ab8a37caf900e55ec8f2

	for (dbnum = 0; dbnum < newdb_arr->ndbs; dbnum++)
	{
		DbInfo	   *new_db = &newdb_arr->dbs[dbnum];
		DbInfo	   *old_db = dbarr_lookup_db(olddb_arr, new_db->db_name);
		FileNameMap *mappings;
		int			n_maps;
		pageCnvCtx *pageConverter = NULL;

<<<<<<< HEAD
		if (!old_db)
			pg_log(ctx, PG_FATAL,
			   "the new cluster database %s was not found in the old cluster\n", new_db->db_name);
		
=======
>>>>>>> 1084f317702e1a039696ab8a37caf900e55ec8f2
		n_maps = 0;
		mappings = gen_db_file_maps(ctx, old_db, new_db, &n_maps, old_pgdata,
									new_pgdata);

		if (n_maps)
		{
			print_maps(ctx, mappings, n_maps, new_db->db_name);

#ifdef PAGE_CONVERSION
			msg = setupPageConverter(ctx, &pageConverter);
#endif
			transfer_single_new_db(ctx, pageConverter, mappings, n_maps);

			pg_free(mappings);
		}
	}

	prep_status(ctx, "");		/* in case nothing printed */
	check_ok(ctx);

	return msg;
}


/*
 * get_pg_database_relfilenode()
 *
 *	Retrieves the relfilenode for a few system-catalog tables.	We need these
 *	relfilenodes later in the upgrade process.
 */
void
get_pg_database_relfilenode(migratorContext *ctx, Cluster whichCluster)
{
	PGconn	   *conn = connectToServer(ctx, "template1", whichCluster);
	PGresult   *res;
	int			i_relfile;

	res = executeQueryOrDie(ctx, conn,
							"SELECT c.relname, c.relfilenode "
							"FROM 	pg_catalog.pg_class c, "
							"		pg_catalog.pg_namespace n "
							"WHERE 	c.relnamespace = n.oid AND "
							"		n.nspname = 'pg_catalog' AND "
							"		c.relname = 'pg_database' "
							"ORDER BY c.relname");

	i_relfile = PQfnumber(res, "relfilenode");
	if (whichCluster == CLUSTER_OLD)
<<<<<<< HEAD
		ctx->old.pg_database_oid = atooid(PQgetvalue(res, 0, i_relfile));
	else
		ctx->new.pg_database_oid = atooid(PQgetvalue(res, 0, i_relfile));
=======
		ctx->old.pg_database_oid = atol(PQgetvalue(res, 0, i_relfile));
	else
		ctx->new.pg_database_oid = atol(PQgetvalue(res, 0, i_relfile));
>>>>>>> 1084f317702e1a039696ab8a37caf900e55ec8f2

	PQclear(res);
	PQfinish(conn);
}


/*
 * transfer_single_new_db()
 *
 * create links for mappings stored in "maps" array.
 */
static void
transfer_single_new_db(migratorContext *ctx, pageCnvCtx *pageConverter,
					   FileNameMap *maps, int size)
{
	int			mapnum;

	for (mapnum = 0; mapnum < size; mapnum++)
	{
		char		old_file[MAXPGPATH];
		char		new_file[MAXPGPATH];
		struct dirent **namelist = NULL;
		int			numFiles;
<<<<<<< HEAD
		bool		seg0_missing;
		struct stat st;
=======
>>>>>>> 1084f317702e1a039696ab8a37caf900e55ec8f2

		/* Copying files might take some time, so give feedback. */

		snprintf(old_file, sizeof(old_file), "%s/%u", maps[mapnum].old_file, maps[mapnum].old);
		snprintf(new_file, sizeof(new_file), "%s/%u", maps[mapnum].new_file, maps[mapnum].new);
		pg_log(ctx, PG_REPORT, OVERWRITE_MESSAGE, old_file);

		/*
		 * Copy/link the relation file to the new cluster
		 */
		unlink(new_file);
<<<<<<< HEAD

		if (!maps[mapnum].missing_seg0_ok || stat(old_file, &st) == 0)
		{
			transfer_relfile(ctx, pageConverter, old_file, new_file,
						 maps[mapnum].old_nspname, maps[mapnum].old_relname,
						 maps[mapnum].new_nspname, maps[mapnum].new_relname,
						 maps[mapnum].gpdb4_heap_conversion_needed,
						 maps[mapnum].has_numerics, maps[mapnum].atts,
						 maps[mapnum].natts, maps[mapnum].type);
			seg0_missing = false;
		}
		else
			seg0_missing = true;
=======
		transfer_relfile(ctx, pageConverter, old_file, new_file,
						 maps[mapnum].old_nspname, maps[mapnum].old_relname,
						 maps[mapnum].new_nspname, maps[mapnum].new_relname);
>>>>>>> 1084f317702e1a039696ab8a37caf900e55ec8f2

		/* fsm/vm files added in PG 8.4 */
		if (GET_MAJOR_VERSION(ctx->old.major_version) >= 804)
		{
			/*
			 * Now copy/link any fsm and vm files, if they exist
			 */
			snprintf(scandir_file_pattern, sizeof(scandir_file_pattern), "%u_", maps[mapnum].old);
			numFiles = pg_scandir(ctx, maps[mapnum].old_file, &namelist, dir_matching_filenames);

			while (numFiles--)
			{
				snprintf(old_file, sizeof(old_file), "%s/%s", maps[mapnum].old_file,
						 namelist[numFiles]->d_name);
				snprintf(new_file, sizeof(new_file), "%s/%u%s", maps[mapnum].new_file,
				  maps[mapnum].new, strchr(namelist[numFiles]->d_name, '_'));

				unlink(new_file);
				transfer_relfile(ctx, pageConverter, old_file, new_file,
						  maps[mapnum].old_nspname, maps[mapnum].old_relname,
<<<<<<< HEAD
								 maps[mapnum].new_nspname, maps[mapnum].new_relname,
								 false, false, NULL, 0, FSM);
=======
						 maps[mapnum].new_nspname, maps[mapnum].new_relname);
>>>>>>> 1084f317702e1a039696ab8a37caf900e55ec8f2

				pg_free(namelist[numFiles]);
			}

			pg_free(namelist);
		}

		/*
		 * Now copy/link any related segments as well. Remember, PG breaks
		 * large files into 1GB segments, the first segment has no extension,
		 * subsequent segments are named relfilenode.1, relfilenode.2,
		 * relfilenode.3, ...  'fsm' and 'vm' files use underscores so are not
		 * copied.
		 */
		snprintf(scandir_file_pattern, sizeof(scandir_file_pattern), "%u.", maps[mapnum].old);
		numFiles = pg_scandir(ctx, maps[mapnum].old_file, &namelist, dir_matching_filenames);

		while (numFiles--)
		{
			snprintf(old_file, sizeof(old_file), "%s/%s", maps[mapnum].old_file,
					 namelist[numFiles]->d_name);
			snprintf(new_file, sizeof(new_file), "%s/%u%s", maps[mapnum].new_file,
				  maps[mapnum].new, strchr(namelist[numFiles]->d_name, '.'));

			unlink(new_file);
			transfer_relfile(ctx, pageConverter, old_file, new_file,
						  maps[mapnum].old_nspname, maps[mapnum].old_relname,
<<<<<<< HEAD
							 maps[mapnum].new_nspname, maps[mapnum].new_relname,
							 maps[mapnum].gpdb4_heap_conversion_needed,
							 maps[mapnum].has_numerics, maps[mapnum].atts,
							 maps[mapnum].natts, maps[mapnum].type);
=======
						 maps[mapnum].new_nspname, maps[mapnum].new_relname);
>>>>>>> 1084f317702e1a039696ab8a37caf900e55ec8f2

			pg_free(namelist[numFiles]);
		}

		pg_free(namelist);
	}
}


/*
 * transfer_relfile()
 *
 * Copy or link file from old cluster to new one.
 */
static void
transfer_relfile(migratorContext *ctx, pageCnvCtx *pageConverter, const char *oldfile,
		 const char *newfile, const char *oldnspname, const char *oldrelname,
<<<<<<< HEAD
				 const char *newnspname, const char *newrelname,
				 bool gpdb4_heap_conversion_needed,
				 bool has_numerics, AttInfo *atts, int natts, RelType type)
{
	const char *msg;

	if (gpdb4_heap_conversion_needed)
	{
		pg_log(ctx, PG_INFO, "copying and converting %s to %s\n", oldfile, newfile);

		if ((msg = convert_gpdb4_heap_file(ctx, oldfile, newfile,
										   has_numerics, atts, natts)) != NULL)
			pg_log(ctx, PG_FATAL, "error while copying %s.%s(%s) to %s.%s(%s): %s\n",
				   oldnspname, oldrelname, oldfile, newnspname, newrelname, newfile, msg);

		return;
	}

=======
				 const char *newnspname, const char *newrelname)
{
	const char *msg;

>>>>>>> 1084f317702e1a039696ab8a37caf900e55ec8f2
	if ((ctx->transfer_mode == TRANSFER_MODE_LINK) && (pageConverter != NULL))
		pg_log(ctx, PG_FATAL, "this migration requires page-by-page conversion, "
			   "you must use copy-mode instead of link-mode\n");

	if (ctx->transfer_mode == TRANSFER_MODE_COPY)
	{
<<<<<<< HEAD
		if (ctx->checksum_mode != CHECKSUM_NONE && type == HEAP)
		{
			pg_log(ctx, PG_INFO, "rewriting \"%s\" to \"%s\"\n",
				   oldfile, newfile);
			rewriteHeapPageChecksum(ctx, oldfile, newfile, newnspname, newrelname);
		}
		else
		{
			pg_log(ctx, PG_INFO, "copying \"%s\" to \"%s\"\n",
				   oldfile, newfile);
			if ((msg = copyAndUpdateFile(ctx, pageConverter, oldfile, newfile, true)) != NULL)
				pg_log(ctx, PG_FATAL, "error while copying %s.%s(%s) to %s.%s(%s): %s\n",
					   oldnspname, oldrelname, oldfile, newnspname, newrelname, newfile, msg);
		}
	}
	else
	{
		pg_log(ctx, PG_INFO, "linking %s to %s\n", oldfile, newfile);
=======
		pg_log(ctx, PG_INFO, "copying %s to %s\n", oldfile, newfile);

		if ((msg = copyAndUpdateFile(ctx, pageConverter, oldfile, newfile, true)) != NULL)
			pg_log(ctx, PG_FATAL, "error while copying %s.%s(%s) to %s.%s(%s): %s\n",
				   oldnspname, oldrelname, oldfile, newnspname, newrelname, newfile, msg);
	}
	else
	{
		pg_log(ctx, PG_INFO, "linking %s to %s\n", newfile, oldfile);
>>>>>>> 1084f317702e1a039696ab8a37caf900e55ec8f2

		if ((msg = linkAndUpdateFile(ctx, pageConverter, oldfile, newfile)) != NULL)
			pg_log(ctx, PG_FATAL,
			   "error while creating link from %s.%s(%s) to %s.%s(%s): %s\n",
				   oldnspname, oldrelname, oldfile, newnspname, newrelname,
				   newfile, msg);
	}
	return;
}
