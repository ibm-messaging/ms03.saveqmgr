             PGM        PARM(&VERSION &LCLQMGRNAM &RMTQMGRNAM +
                          &FILENAME)

             DCL        VAR(&VERSION) TYPE(*CHAR) LEN(1)
             DCL        VAR(&LCLQMGRNAM) TYPE(*CHAR) LEN(48)
             DCL        VAR(&RMTQMGRNAM) TYPE(*CHAR) LEN(48)
             DCL        VAR(&FILENAME) TYPE(*CHAR) LEN(33)


             CALL       PGM(SAVEQMGR) PARM('-v' &VERSION '-m' +
                          &LCLQMGRNAM '-r' &RMTQMGRNAM '-f' &FILENAME)

