-module(persistent_term).

-export([erase/1, get/0, get/1, get/2, info/0, put/2]).

-type key() :: term().
-type value() :: term().
-ifdef(no_strict_map_type).
-type info() :: #{count => non_neg_integer(), memory => non_neg_integer()}.
-else.
-type info() :: #{count := non_neg_integer(), memory := non_neg_integer()}.
-endif.

-compile(no_inline).

-on_load(load_nif/0).
-define(PERSISTENT_TERM_NIF_VSN, 1).

load_nif() ->
    P = case code:priv_dir(rar) of
            {error, bad_name} ->
                D1 = filename:join([".", "priv", "lib"]),
                case filelib:is_dir(D1) of
                    true -> D1;
                    _ ->
                        D2 = [$.|D1],
                        case filelib:is_dir(D2) of
                            true -> D2;
                            _ -> "."
                        end
                end;
            D -> D
        end,
    E = file:native_name_encoding(),
    L = filename:join(P, "persistent_term"),
    erlang:load_nif(L, {?PERSISTENT_TERM_NIF_VSN, unicode:characters_to_binary(L, E, E)}).

-spec erase(Key::key()) -> boolean().
erase(_Key) -> erlang:nif_error(undef).

-spec get() -> [{key(), value()}].
get() -> erlang:nif_error(undef).

-spec get(Key::key()) -> value().
get(_Key) -> erlang:nif_error(undef).

-spec get(Key::key(), Default::value()) -> value().
get(_Key, _Default) -> erlang:nif_error(undef).

-spec put(Key::key(), Value::value()) -> ok.
put(_Key, _Value) -> erlang:nif_error(undef).

-spec info() -> info().
info() -> erlang:nif_error(undef).
