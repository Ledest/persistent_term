#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "erl_nif.h"

static ERL_NIF_TERM atom_ok, atom_true, atom_false, atom_count, atom_memory;

static ErlNifEnv* pte;
static ERL_NIF_TERM map;

static ERL_NIF_TERM put(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
	enif_make_map_put(pte, map, argv[0], argv[1], &map);
	return atom_ok;
}

static ERL_NIF_TERM erase(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
	ERL_NIF_TERM m;

	enif_make_map_remove(pte, map, argv[0], &m);
	if (m == map)
		return atom_false;
	map = m;
	return atom_true;
}

static ERL_NIF_TERM get(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
	ERL_NIF_TERM v;

	if (!argc) {
		ErlNifMapIterator i;
		ERL_NIF_TERM k, l;

		enif_map_iterator_create(pte, map, &i, ERL_NIF_MAP_ITERATOR_LAST);
		for (enif_map_iterator_create(pte, map, &i, ERL_NIF_MAP_ITERATOR_LAST), l = enif_make_list(env, 0);
		     enif_map_iterator_get_pair(pte, &i, &k, &v);
		     enif_map_iterator_prev(pte, &i)) {
			l = enif_make_list_cell(env,
						enif_make_tuple2(env, enif_make_copy(env, k), enif_make_copy(env, v)),
						l);
		}
		enif_map_iterator_destroy(pte, &i);
		return l;
	}
	if (enif_get_map_value(pte, map, argv[0], &v))
		return enif_make_copy(env, v);
	return argc == 2 ? argv[1] : enif_make_badarg(env);
}

static ERL_NIF_TERM info(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
	size_t c;
	uint64_t memory = 0;
	ERL_NIF_TERM m = enif_make_new_map(env);

	enif_get_map_size(pte, map, &c);
	enif_make_map_put(env, m, atom_count, enif_make_uint(env, c), &m);
	enif_make_map_put(env, m, atom_memory, enif_make_uint64(env, memory), &m);
	return m;
}

static int load(ErlNifEnv* env, void** priv, ERL_NIF_TERM info)
{
	pte = enif_alloc_env();
	atom_ok = enif_make_atom(pte, "ok");
	atom_true = enif_make_atom(pte, "true");
	atom_false = enif_make_atom(pte, "false");
	atom_count = enif_make_atom(pte, "count");
	atom_memory = enif_make_atom(pte, "memory");
	map = enif_make_new_map(pte);

	return 0;
}

static int upgrade(ErlNifEnv* env, void** priv, void** old_priv, ERL_NIF_TERM info)
{
	return 0;
}

static ErlNifFunc nif_functions[] = {
	{"erase", 1, erase},
	{"get", 0, get},
	{"get", 1, get},
	{"get", 2, get},
	{"put", 2, put},
	{"info", 0, info}
};

ERL_NIF_INIT(persistent_term, nif_functions, &load, NULL, &upgrade, NULL);
