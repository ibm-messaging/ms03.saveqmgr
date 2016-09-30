# ms03.saveqmgr
=============

A program to extract the configuration of IBM MQ Objects from a Queue Manager as an MQSC Script file.

This was originally released as SupportPac MS03 Saveqmgr.

There are no binary files shipped with this. If you want to use this program, you must have a compiler for your OS installed and use make or nmake (on Windows) to build a binary.

# NOTICE OF RETIRMENT

THIS CODE IS OBSOLETE AND SHOULD NOT BE USED.
ANY IBM MQ VERSION VERSION 7.1 OR LATER SHOULD USE DMPMQCFG OR THE RELEVANT COMMAND ON iSeries or zOS. 

# SUPPORT STATEMENT 
This code is not supported.  

Any issues or questions should be created as github issues, with no guarantees provided as to any response or fixes. 


# Possible Uses

Customers can extract an MQSC script file to backup and recreate MQ Objects defined on a particular queue manager.  

This program should only be used with MQ versions 7.0.x and earlier. Any version of IBM MQ later than that includs the dmpmqcfg program that will have information about IBM MQ 7.1 or later MQ Objects. 

This does not know about any IBM MQ 7.1 or later objects, and thus will not extract their definitions and will show errors that it can't understand them.

Pull requests
=============
Contributions to this package can be accepted under the terms of the 
IBM Contributor License Agreement, found in the file CLA.md of this repository.

When submitting a pull request, you must include a statement stating you accept the terms in CLA.md.

Details
=============

Initial Author : Chris Petty
Maintainer Emeritus : Wayne Schutz 
Last Maintainer : Jeff Lowrey

Last Release : May 2009

Released to GitHub : September 2016
