             CMD        PROMPT('SAVEQMGR: SAVE OBJECTS DEFS')

             PARM       KWD(VERSION) TYPE(*CHAR) LEN(3) RSTD(*YES) +
                          DFT(700) VALUES(520 530 600 700) EXPR(*YES) +
                          PROMPT('Version of MQSC to Generate')

             PARM       KWD(LCLQMGRNAM) TYPE(*CHAR) LEN(48) +
                          EXPR(*YES) PROMPT('Local QMgr Name (for MQCONN)')

             PARM       KWD(RMTQMGRNAM) TYPE(*CHAR) LEN(48) +
                          EXPR(*YES) PROMPT('Remote QMgr Name (XMITQ name)')

             PARM       KWD(FILENAME) TYPE(*CHAR) LEN(33) +
                          DFT('*CURLIB/QMQSC(SAVEQMGR)') EXPR(*YES) +
                          PROMPT('MQSC Output file')

             PARM       KWD(OUTPUT) TYPE(*CHAR) LEN(8) +
                          DFT(*APPEND) PROMPT('Output Mode') +
                          VALUES(*APPEND *REPLACE) RSTD(*YES)

             PARM       KWD(RDEFSYS) TYPE(*CHAR) LEN(4) +
                          DFT(*YES) PROMPT('Redefine system objects') +
                          VALUES(*YES *NO) RSTD(*YES)

             PARM       KWD(MAKEAUTH) TYPE(*CHAR) LEN(4) +
                          DFT(*NO) PROMPT('Make GRTMQMAUT commands') +
                          VALUES(*YES *NO) RSTD(*YES)

             PARM       KWD(AUTHFN) TYPE(*CHAR) LEN(33) +
                          EXPR(*YES) PROMPT('GRTMQMAUT Command Ouput file') +
                          DFT('*CURLIB/QMAUT(SAVEQMGR)')

             PARM       KWD(PERMDYN) TYPE(*CHAR) LEN(4) +
                          DFT(*NO) +
                          PROMPT('Make qlocal defs from PERMDYN') +
                          VALUES(*YES *NO) RSTD(*YES)

             PARM       KWD(IGNDAMAGED) TYPE(*CHAR) LEN(4) +
                          DFT(*NO) +
                          PROMPT('Ignore DAMAGED OBJECT errors') +
                          VALUES(*YES *NO) RSTD(*YES)



