-module(line).

-export([start/1, send/2, bounce/3, stop/1]).

% Se crean N procesos en una línea
start(N) ->
    Pids = [spawn(fun() -> pid_waiting() end) || _ <- lists:seq(0, N - 1)],% Crea una lista de Pids con N procesos, de 0 a N-1 y hace que cada proceso se quede esperando a ser configurado
    lists:foreach(
        fun(Num) ->
            Pid = lists:nth(Num + 1, Pids), % Obtiene el Pid del proceso actual (el pid actual es Num + 1 porque las listas en Erlang empiezan en 1)
            PrevPid = if Num == 0 -> none; true -> lists:nth(Num, Pids) end, % Obtiene el Pid del proceso anterior, o none si es el primero
            NextPid = if Num == N - 1 -> none; true -> lists:nth(Num + 2, Pids) end, % Obtiene el Pid del proceso siguiente, o none si es el último
            Pid ! {config, Num, PrevPid, NextPid} % Envía la configuración a cada proceso
        end,
        lists:seq(0, N - 1)
    ),
    hd(Pids). % Se obtiene el Pid del primer proceso

% Se envía un mensaje a través de la línea
send(Pid, Msg) ->
    Pid ! {send, Msg},
    ok.

% Se envía un mensaje que rebota Times veces
bounce(Pid, Msg, Times) ->
    Pid ! {bounce, Msg, Times},
    ok.

% Se detienen todos los procesos en la línea
stop(Pid) ->
    Pid ! stop,
    ok.

% Hace que cada proceso espere a ser configurado para que ninguno empiece sin estar listo
pid_waiting() ->
    receive
        {config, Num, PrevPid, NextPid} ->
            process_loop(Num, PrevPid, NextPid)
    end.

% Bucle principal de cada proceso
process_loop(Num, PrevPid, NextPid) ->
    receive
        {send, Msg} ->
            io:format("~w received message ~w~n", [Num, Msg]),
            case NextPid of % Se envia al siguiente proceso hasta llegar al ultimo, donde para
                none -> ok;
                _ -> NextPid ! {send, Msg}
            end,
            process_loop(Num, PrevPid, NextPid);
        
        {bounce, Msg, Times} ->
            io:format("~w received message ~w~n", [Num, Msg]),
            NewTimes = Times - 1, % Se reduce el numero de rebotes restantes
            case NextPid of
                none ->
                    % Cuando llega al final, si quedan rebotes va hacia atras, sino para
                    if NewTimes > 0 ->
                        PrevPid ! {bounce_back, Msg, NewTimes};
                    true ->
                        ok
                    end;
                _ ->
                    % Sigue hacia delante
                    NextPid ! {bounce, Msg, NewTimes}
            end,
            process_loop(Num, PrevPid, NextPid); % Se llama recursivamente a la función para seguir esperando mensajes
        
        % Mismo que en bounce pero en sentido contrario
        {bounce_back, Msg, Times} ->
            io:format("~w received message ~w~n", [Num, Msg]),
            NewTimes = Times - 1,
            case PrevPid of
                none ->
                    % Si llega al principio, si quedan rebotes va hacia adelante, sino para
                    if NewTimes > 0 ->
                        NextPid ! {bounce, Msg, NewTimes};
                    true ->
                        ok
                    end;
                _ ->
                    % Continuar hacia atrás
                    PrevPid ! {bounce_back, Msg, NewTimes}
            end,
            process_loop(Num, PrevPid, NextPid);
        
        stop ->
            case NextPid of % Se envia el mensaje stop uno por uno en la línea hasta llegar al ultimo, donde para
                none -> ok;
                _ -> NextPid ! stop
            end
    end.