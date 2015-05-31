
tmpfile=/tmp/db2_create_db.$$
logfile=db2_create_db.log
trap "rm -f $tmpfile" EXIT HUP QUIT

#----------------------------------
#    functions
#----------------------------------

display_usage ()
{

cat <<EOF

   usage: db2_create_db.ksh GOAHEAD - if run by db-owner
   
         o should be run by individual db owner when creating database

EOF
}

mymount()
{
# $1 - mount point
# $2 - [owner]
# $3 - [permission]

  tfile=/tmp/mymount

  if [ ! -d $1 ]
  then
     echo "mkdir -p $1.."
     mkdir -p $1
  fi

  echo "mount $1.."
  if mount $1 >$tfile 2>&1
  then
     :
  else
     if [ $? -ne 255 ] #-- error but not busy
     then
        cat /tmp/mymount
     fi
  fi

  if [ "$2" != "" ]
  then
     echo "chown -R $2 $1.."
     chown -R $2 $1
  fi
 
  if [ "$3" != "" ]
  then
     echo "chmod -R $3 $1.."
     chmod -R $3 $1
  fi
 
}

my_mkdir ()
{
# $1 - mount point
# $2 - [owner]
# $3 - [permission]

  if [ ! -d $1 ]
  then
     echo "mkdir -p $1.."
     mkdir -p $1
  fi

  if [ "$2" != "" ]
  then
     echo "chown -R $2 $1.."
     chown -R $2 $1
  fi
 
  if [ "$3" != "" ]
  then
     echo "chmod -R $3 $1.."
     chmod -R $3 $1
  fi
 
}

calldb2 ()
{
# $1 - cmd

  rc=0
  case $todebug in
    Y|y ) echo "$1" ;;
      * ) 
          set -x
          db2 "$1" 
          rc=$?
          set +x
          ;;
  esac

  return $rc
}

calldb2_1 ()
{
# $1 - file

  case $todebug in
    Y|y ) cat $1 ;;
      * ) 
          set -x
          db2 -tvf $1
          set +x
          ;;
  esac
}

create_db ()
{
# $1 - db name
# $2 - db directory
# $3 - db description
# $4 - catalog directory

echo "start to create database $1.."

if [ "$todebug" = "N" ]
then
   if [ ! -d "$2" ] 
   then
      set -x
      mkdir -p $2
      set +x
   fi
   if [ ! -d "$4" ] 
   then
      set -x
      mkdir -p $4
      set +x
   fi
fi

#calldb2 \
#"create database $1 on $2 numsegs 4 catalog tablespace managed by system using ('$4') with \"$3\""

calldb2 \
"create database $realname on $2 numsegs 4 catalog tablespace managed by system using ('$4') with \"$3\"" || return 12

calldb2 \
"catalog database $realname as $1 on $2" || return 15

}

create_nodegroups ()
{
# $1 - db name
# $2 - main nodegroup name

  echo "start to create nodegroups.."

  cat >$tmpfile <<EOF
connect to $1;
create nodegroup $2 on nodes($main_ng_nodes);
EOF

#  if [ "$mode" = "EEE" ]
#  then
#  cat >>$tmpfile <<EOF
#create nodegroup nodegroup_0 on node(0);
#create nodegroup nodegroup_1 on node(1);
#create nodegroup nodegroup_2 on node(2);
#create nodegroup nodegroup_3 on node(3);
#create nodegroup nodegroup_4 on node(4);
#EOF
#  fi

  calldb2_1 $tmpfile
}

create_tbsp ()
{
# $1 - db name
# $2 - tbsp name
# $3 - keyword - "" or TEMPORARY
# $4 - start at partitions
# $5 - end at partitions
# $6,... - tbsp dir (format= nodeid:path)

  db=$1; shift
  tbsp=$1; shift
  key=$1; shift
  integer start_at_partition=$1; shift
  integer end_at_partition=$1; shift

  echo "start to create tbsp $tbsp.."

  cat >$tmpfile <<EOF
connect to $db;
create $key tablespace $tbsp pagesize 4k managed by system
EOF

  integer i=0
  while ((i<=end_at_partition))
  do
      if ((i>=start_at_partition))
      then
         case $mode in
            EE ) 
                 if ((i==start_at_partition))
                 then
                     if ((i==end_at_partition))
                     then
                        echo "using ('${1#*:}')" >>$tmpfile 
                     else
                        echo "using ('${1#*:}'" >>$tmpfile 
                     fi
                 else
                    if ((i==end_at_partition))
                    then
                        echo "      ,'${1#*:}')" >>$tmpfile 
                    else
                        echo "      ,'${1#*:}'" >>$tmpfile 
                    fi
                 fi
                 ;;
           EEE ) echo "using ('${1#*:}') on node(${1%:*})" >>$tmpfile ;;
         esac
      fi
      i=i+1
      shift
  done

  cat >>$tmpfile <<EOF
  extentsize $extentsize
  prefetchsize $prefetchsize
  dropped table recovery off;
EOF

  calldb2_1 $tmpfile
}

drop_tbsp ()
{
# $1 - db name
# $2 - tbsp name

  echo "start to drop tbsp $2.."

  cat >$tmpfile <<EOF
connect to $1;
drop tablespace $2;
EOF

  calldb2_1 $tmpfile
}

change_log ()
{
# $1     - db name
# $2     - start at partition
# $3     - end at partition
# $4,... - log dir (format= nodeid:path)

  echo "start to change the DB2 logs.."

  db=$1; shift
  integer start_at_partition=$1; shift
  integer end_at_partition=$1; shift

  if [ "$mode" = "EE" ]
  then
     newlogpath="${1#*:}"
     [ -d $newlogpath ] || mkdir -p $newlogpath
     calldb2 "update db cfg for $db using newlogpath $newlogpath"
     return
  fi

  integer i=0
  while ((i<=end_at_partition))
  do
      if ((i>=start_at_partition))
      then
          if [ "$mode" = "EEE" ]
          then
             calldb2 "terminate"
             cmd="export DB2NODE=${1%:*}"
             [ "$todebug" = "Y" ] && echo "$cmd"
             eval $cmd
          fi
          echo "DB2NODE:$DB2NODE.."
          newlogpath="${1#*:}"
          [ -d $newlogpath ] || mkdir -p $newlogpath
          calldb2 "update db cfg for $db using newlogpath $newlogpath"
      fi
      i=i+1
      shift
  done
}

main_logic ()
{
    extentsize=32
    prefetchsize=64

    node0=dp0
    node1=dp1
    node2=dp2
    node3=dp3
    node4=dp4

#-- derived

    main_ngname=${dbshort}nodes

    eval dbdir=/$rootdir/$"node$start_at_partition"/$dbname   
    eval catdir=/$rootdir/$"node$start_at_partition"/catalog

    newtemp=tempspace2
    temptbsp_dir0=/$rootdir/$node0/$newtemp #-- at node 0
    temptbsp_dir1=/$rootdir/$node1/$newtemp #-- at node 1
    temptbsp_dir2=/$rootdir/$node2/$newtemp #-- at node 2
    temptbsp_dir3=/$rootdir/$node3/$newtemp #-- at node 3
    temptbsp_dir4=/$rootdir/$node4/$newtemp #-- at node 4

    newuser=userspace2
    usertbsp_dir0=/$rootdir/$node0/$newuser #-- at node 0
    usertbsp_dir1=/$rootdir/$node1/$newuser #-- at node 1
    usertbsp_dir2=/$rootdir/$node2/$newuser #-- at node 2
    usertbsp_dir3=/$rootdir/$node3/$newuser #-- at node 3
    usertbsp_dir4=/$rootdir/$node4/$newuser #-- at node 4

    if [ "$mode" = "EE" ]
    then
       log_dir0=/$rootdir/logs
    else
       log_dir0=/$rootdir/$node0/logs #-- at node 0
       log_dir1=/$rootdir/$node1/logs #-- at node 1
       log_dir2=/$rootdir/$node2/logs #-- at node 2
       log_dir3=/$rootdir/$node3/logs #-- at node 3
       log_dir4=/$rootdir/$node4/logs #-- at node 4
    fi

#== create database ==#
 
    create_db $dbname $dbdir "$dbdesc" $catdir || exit 1

    [ "$mode" = "EEE" ] && create_nodegroups $dbname $main_ngname

#== handle temporary tbsp ==#
 
    create_tbsp $dbname $newtemp "TEMPORARY" $start_at_partition $end_at_partition \
    0:$temptbsp_dir0 1:$temptbsp_dir1 2:$temptbsp_dir2 3:$temptbsp_dir3 4:$temptbsp_dir4
    drop_tbsp $dbname tempspace1

#== handle user tbsp ==#
 
    drop_tbsp $dbname userspace1
    create_tbsp $dbname $newuser "" $start_at_partition $end_at_partition \
    0:$usertbsp_dir0 1:$usertbsp_dir1 2:$usertbsp_dir2 3:$usertbsp_dir3 4:$usertbsp_dir4

#== handle db2 log file ==#

    change_log $dbname $start_at_partition $end_at_partition \
    0:$log_dir0 1:$log_dir1 2:$log_dir2 3:$log_dir3 4:$log_dir4
}


msdw_add_tbsp ()
{
#-- $1 instance owner name

  calldb2 "connect to $1"
  calldb2 "create regular tablespace main_data_tbsp in nodegroup ibmdefaultgroup pagesize 4096 managed by system using ('/$1/dp1/data/main')"

}

msdw_config_fed ()
{
# $1 - instance name
# $2 - [local]

>$tmpfile

  cat >>$tmpfile <<EOF
connect to $1;
EOF

  if [ "$2" = "" ]
  then
      cat >>$tmpfile <<EOF
catalog tcpip node devaida remote odem8002 server 50000;
EOF
  else
      cat >>$tmpfile <<EOF
catalog local node devaida instance db2inst1;
EOF
  fi

  cat >>$tmpfile <<EOF
create wrapper drda;
create server devaida type db2/6000 version 7.1
  wrapper drda
  authorization db2inst1 password db2admin
  options ( node 'devaida', dbname 'aida' );
create user mapping for $1 server devaida options
    (remote_authid 'db2inst1', remote_password 'db2admin');
create nickname msdw.t_protein for devaida.aida.t_protein;
create nickname msdw.id for devaida.aida.id;
select count(1) from msdw.t_protein;
EOF

  calldb2_1 $tmpfile

}

#--------------------------------------------
#  :main logic
#--------------------------------------------

# userid/groupid for "prdseqhi" :
#
# o userid=prdseqhi, home dir=/seqh/home/prdseqhi
# o groupid=prdsqhig - instance id gp 
#   groupid=prdsqhfg - fenced user gp
#
# ================================
#
# userid/groupid for "devcarpi" :
#
# o userid=devcarpi, home dir=/devcarp/home/devcarpi
# o groupid=devcarig - instance id gp 
#   groupid=devcarfg - fenced user gp
# o lv: 
#       devcarp_homelv     /devcarp
#       devcarp_datlv01    /devcarp/dp1
#       devcarp_datlv02    /devcarp/dp2
#       devcarp_datlv03    /devcarp/dp3
#       devcarp_datlv04    /devcarp/dp4
#       devcarp_loglv01    /devcarp/dp1/logs
#       devcarp_loglv02    /devcarp/dp2/logs
#       devcarp_loglv03    /devcarp/dp3/logs
#       devcarp_loglv04    /devcarp/dp4/logs
# o tbsp: /devcarp/dp1/data
#         /devcarp/dp1/catalog
#         /devcarp/dp1/index
#         /devcarp/dp1/long
#         /devcarp/dp1/logs
#         /devcarp/dp1/userspace2
#         /devcarp/dp1/tempspace2
#         /devcarp/dp1/carp/devcarpi
#
#         /devcarp/dp2/data
#         /devcarp/dp2/index
#         /devcarp/dp2/long
#         /devcarp/dp2/logs
#         /devcarp/dp2/userspace2
#         /devcarp/dp2/tempspace2
#

if [ "$1" = "-h" ]
then
   display_usage
   exit 0
fi

if [ "$1" = "GOAHEAD" ]
then
   todebug=N
else
   todebug=Y
fi

case `id -nu` in
   tstseqhi )
              realname=tstseqh
              dbname=seqhound
              dbshort=tstseqh
              dbdesc="Seqhound - Testing"
              rootdir=seqh09

              #-- mode=EEE
              #-- start_at_partition=0
              #-- end_at_partition=4
              #-- main_ng_nodes="1,2,3,4"

              mode=EE
              start_at_partition=1
              end_at_partition=4
              main_ng_nodes="0"

              main_logic 2>&1 | tee $logfile
              ;;
   prdseqhi )
              realname=prdseqh
              dbname=seqhound
              dbshort=seqh
              dbdesc="Seqhound - Production"
              rootdir=$dbshort

              #-- mode=EEE
              #-- start_at_partition=0
              #-- end_at_partition=4
              #-- main_ng_nodes="1,2,3,4"

              mode=EE
              start_at_partition=1
              end_at_partition=4
              main_ng_nodes="0"

              main_logic 2>&1 | tee $logfile
              ;;
   devseqhi )
              realname=devseqh
              dbname=seqhound
              dbshort=devseqh
              dbdesc="Seqhound - Dev"
              rootdir=$dbshort

              #-- mode=EEE
              #-- start_at_partition=0
              #-- end_at_partition=4
              #-- main_ng_nodes="1,2,3,4"

              mode=EE
              start_at_partition=1
              end_at_partition=4
              main_ng_nodes="0"

              main_logic 2>&1 | tee $logfile
              ;;
   prdmedli )
              realname=prdmedl
              dbname=medline
              dbshort=medl
              dbdesc="Medline - Production"
              rootdir=$dbshort

              #-- mode=EEE
              #-- start_at_partition=0
              #-- end_at_partition=4
              #-- main_ng_nodes="1,2,3,4"

              mode=EE
              start_at_partition=1
              end_at_partition=1
              main_ng_nodes="0"

              main_logic 2>&1 | tee $logfile
              ;;
    testeee )
              realname=testeee
              dbname=testeee
              dbshort=eee
              dbdesc="EEE - testing"
              rootdir=usr/testeee

              #-- mode=EEE
              #-- start_at_partition=0
              #-- end_at_partition=4
              #-- main_ng_nodes="1,2,3,4"

              mode=EEE
              start_at_partition=0
              end_at_partition=3
              main_ng_nodes="0,1,2,3"

              main_logic 2>&1 | tee $logfile
              ;;
    testee  )
              realname=testee
              dbname=testee
              dbshort=ee
              dbdesc="EE - testing"
              rootdir=usr/testee

              #-- mode=EEE
              #-- start_at_partition=0
              #-- end_at_partition=4
              #-- main_ng_nodes="1,2,3,4"

              mode=EE
              start_at_partition=1
              end_at_partition=1
              main_ng_nodes="0"

              main_logic 2>&1 | tee $logfile
              ;;
   devcarpi )
              realname=devcarp
              dbname=carp
              dbshort=carp
              dbdesc="Carp - Development"
              rootdir=dev$dbshort

              #-- mode=EEE
              #-- start_at_partition=0
              #-- end_at_partition=4
              #-- main_ng_nodes="1,2,3,4"

              mode=EE
              start_at_partition=1
              end_at_partition=4
              main_ng_nodes="0"

              main_logic 2>&1 | tee $logfile
              ;;
     dtmsdw )
              realname=dtmsdw
              dbname=msdw
              dbshort=dtmsdw

              # do not make the desc too long. u will get "db connection" not exist msg
              #dbdesc="MassSpec Data Warehouse - Development"

              dbdesc="Tor MSDW Dev"
              rootdir=$dbshort

              #-- mode=EEE
              #-- start_at_partition=0
              #-- end_at_partition=4
              #-- main_ng_nodes="1,2,3,4"

              mode=EE
              start_at_partition=1
              end_at_partition=1
              main_ng_nodes="0"

              main_logic 2>&1 | tee $logfile
              msdw_add_tbsp $realname 2>&1 | tee $logfile
              msdw_config_fed $realname 2>&1 | tee $logfile
              ;;
     tcmsdw )
              realname=tcmsdw
              dbname=msdw
              dbshort=tcmsdw

              # do not make the desc too long. u will get "db connection" not exist msg
              #dbdesc="MassSpec Data Warehouse - Development"

              dbdesc="Char MSDW Test"
              rootdir=$dbshort

              #-- mode=EEE
              #-- start_at_partition=0
              #-- end_at_partition=4
              #-- main_ng_nodes="1,2,3,4"

              mode=EE
              start_at_partition=1
              end_at_partition=1
              main_ng_nodes="0"

              main_logic 2>&1 | tee $logfile
              msdw_add_tbsp $realname 2>&1 | tee $logfile
              msdw_config_fed $realname 2>&1 | tee $logfile
              ;;
     dcmsdw )
              realname=dcmsdw
              dbname=msdw
              dbshort=dcmsdw

              # do not make the desc too long. u will get "db connection" not exist msg
              #dbdesc="MassSpec Data Warehouse - Development"

              dbdesc="Char MSDW Dev"
              rootdir=$dbshort

              #-- mode=EEE
              #-- start_at_partition=0
              #-- end_at_partition=4
              #-- main_ng_nodes="1,2,3,4"

              mode=EE
              start_at_partition=1
              end_at_partition=1
              main_ng_nodes="0"

              #main_logic 2>&1 | tee $logfile
              #msdw_add_tbsp $realname 2>&1 | tee $logfile
              msdw_config_fed $realname 2>&1 | tee $logfile
              ;;
     ttmsdw )
              realname=ttmsdw
              dbname=msdw
              dbshort=ttmsdw

              # do not make the desc too long. u will get "db connection" not exist msg
              #dbdesc="MassSpec Data Warehouse - Development"

              dbdesc="Tor MSDW Test"
              rootdir=$dbshort

              #-- mode=EEE
              #-- start_at_partition=0
              #-- end_at_partition=4
              #-- main_ng_nodes="1,2,3,4"

              mode=EE
              start_at_partition=1
              end_at_partition=1
              main_ng_nodes="0"

              main_logic 2>&1 | tee $logfile
              msdw_add_tbsp $realname 2>&1 | tee $logfile
              msdw_config_fed $realname 2>&1 | tee $logfile
              ;;
     domsdw )
              realname=domsdw
              dbname=msdw
              dbshort=domsdw

              # do not make the desc too long. u will get "db connection" not exist msg
              #dbdesc="MassSpec Data Warehouse - Development"

              dbdesc="Ode MSDW Dev"
              rootdir=$dbshort

              #-- mode=EEE
              #-- start_at_partition=0
              #-- end_at_partition=4
              #-- main_ng_nodes="1,2,3,4"

              mode=EE
              start_at_partition=1
              end_at_partition=1
              main_ng_nodes="0"

              #main_logic 2>&1 | tee $logfile
              #msdw_add_tbsp $realname 2>&1 | tee $logfile
              msdw_config_fed $realname local 2>&1 | tee $logfile
              ;;
     tomsdw )
              realname=tomsdw
              dbname=msdw
              dbshort=tomsdw

              # do not make the desc too long. u will get "db connection" not exist msg
              #dbdesc="MassSpec Data Warehouse - Development"

              dbdesc="Ode MSDW Test"
              rootdir=$dbshort

              #-- mode=EEE
              #-- start_at_partition=0
              #-- end_at_partition=4
              #-- main_ng_nodes="1,2,3,4"

              mode=EE
              start_at_partition=1
              end_at_partition=1
              main_ng_nodes="0"

              main_logic 2>&1 | tee $logfile
              msdw_add_tbsp $realname 2>&1 | tee $logfile
              msdw_config_fed $realname local 2>&1 | tee $logfile
              ;;
    devgseq )
              realname=devgseq
              dbname=geneseq
              dbshort=devgseq

              # do not make the desc too long. u will get "db connection" not exist msg
              #dbdesc="MassSpec Data Warehouse - Development"

              dbdesc="GeneSeq Dev"
              rootdir=tsm01/$dbshort

              #-- mode=EEE
              #-- start_at_partition=0
              #-- end_at_partition=4
              #-- main_ng_nodes="1,2,3,4"

              mode=EE
              start_at_partition=1
              end_at_partition=1
              main_ng_nodes="0"

              main_logic 2>&1 | tee $logfile
              ;;
   db2inst1 )
              #--- odecws01. instance db2inst1
              #
              realname=inv
              dbname=inv
              dbshort=inv

              # do not make the desc too long. u will get "db connection" not exist msg
              #dbdesc="MassSpec Data Warehouse - Development"

              dbdesc="Inventory database"
              rootdir=$dbshort

              #-- mode=EEE
              #-- start_at_partition=0
              #-- end_at_partition=4
              #-- main_ng_nodes="1,2,3,4"

              mode=EE
              start_at_partition=1
              end_at_partition=1
              main_ng_nodes="0"

              #main_logic 2>&1 | tee $logfile
              set -x
              db2 create database $dbname with "$dbdesc"
              ;;
    devmsdw )
              realname=devmsdw
              dbname=msdw
              dbshort=msdw

              # do not make the desc too long. u will get "db connection" not exist msg
              #dbdesc="MassSpec Data Warehouse - Development"

              dbdesc="MassSpec DW Dev"
              rootdir=$dbshort

              #-- mode=EEE
              #-- start_at_partition=0
              #-- end_at_partition=4
              #-- main_ng_nodes="1,2,3,4"

              mode=EE
              start_at_partition=1
              end_at_partition=1
              main_ng_nodes="0"

              main_logic 2>&1 | tee $logfile
              ;;
    devmsdc )
              realname=devmsdc
              dbname=msdw
              dbshort=msdc

              # do not make the desc too long. u will get "db connection" not exist msg
              #dbdesc="MassSpec Data Warehouse - Development"

              dbdesc="MS DW Dev - CV"
              rootdir=$dbshort

              #-- mode=EEE
              #-- start_at_partition=0
              #-- end_at_partition=4
              #-- main_ng_nodes="1,2,3,4"

              mode=EE
              start_at_partition=1
              end_at_partition=1
              main_ng_nodes="0"

              main_logic 2>&1 | tee $logfile
              ;;
esac


cat <<EOF

logfile : $logfile..
note:

  o u may want to change the ulimit before you try to create tables with big files
  o make sure the file system u use for the database is large file enabled

EOF

#---- end
