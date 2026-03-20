-module(ring).

-export([start/1, send/3, stop/1]).

start(N) ->
    {error, not_implemented}.

send(Pid, N, Msg) ->
    {error, not_implemented}.

stop(Pid) ->
    {error, not_implemented}.