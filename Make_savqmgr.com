$! Simple DCL script to build SAVEQMGR.EXE on VMS
$!
$ SvVerify = F$Verify(1)
$ define mqs_include sys$share:[mqs_include]
$ CFlags = "/decc /warnings=(enable=all, " + -
                "disable=(misalgndstrct, switchlong, ignorecallval), noinform) " + -
                "/include_directory=mqs_include:"
$ CC 'CFlags' SaveQmgr
$ CC 'CFlags' Channel
$ CC 'CFlags' MqUtils
$ CC 'CFlags' Process
$ CC 'CFlags' NameList
$ CC 'CFlags' AuthInfo
$ CC 'CFlags' Qmgr
$ CC 'CFlags' Queue
$!
$ Open/Write Tmp saveqmgr.opt
$ Write Tmp "sys$share:mqm/share"
$ Close Tmp
$!
$ Link /Trace /Nomap /Executable=SaveQmgr.exe SaveQmgr.Obj, -
        Channel.Obj, MqUtils.Obj, Process.Obj, NameList.Obj, Qmgr.Obj, -
        Queue.Obj, AuthInfo.Obj, SaveQmgr/Options
$!
$ If .Not. SvVerify Then $ Set Noverify
$ Exit
