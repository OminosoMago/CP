-module(queue).

-export([empty/0, insert/2, remove/1]).

%%Crear lista vacia
empty() ->
    {[], []}.

%%Insercion
insert({Front, Back}, Elem) ->
    {Front, [Elem | Back]}.

%%Borrar si esta vacia
remove({[], []}) ->
    empty;

%%Borrar normal
remove({[H | T], Back}) ->
    {ok, H, {T, Back}};

%%Borrar cuando el primer elemento si la lista "front" esta vacia
remove({[], Back}) ->
    NewFront = lists:reverse(Back),
    case NewFront of
        [] ->
            empty;
        [H | T] ->
            {ok, H, {T, []}}
    end.
