echo start

MAKEDIR=`pwd`
echo make directory is: $MAKEDIR

echo; echo "== make.bzlib =="
cd ../../../bzip2; make -f make.bzlib clean
cd $MAKEDIR

echo; echo "make.deslib"
cd ../../../deslib; make -f make.deslib clean
cd $MAKEDIR

echo; echo "make.rsareflib"
cd ../../../rsaref; make -f make.rsareflib clean
cd $MAKEDIR

echo; echo "make.slrilib_cb"
cd ../../../slri/lib; make -f make.slrilib_cb clean
cd $MAKEDIR

echo; echo "make.slrilib"
cd ../../../slri/lib; make -f make.slrilib clean
cd $MAKEDIR

echo; echo "make.taskapi"
cd ../../../slri/mobidick/build/taskapi; make -f make.taskapi clean
cd $MAKEDIR

echo; echo "make.slritilib"
cd ../../../slri/textindex; make -f make.slritilib clean
cd $MAKEDIR

echo; echo "make.shoundlocllib"
cd ../../../slri/seqhound/src; make -f make.shoundlocllib clean
cd $MAKEDIR

echo; echo "make.shoundremlib"
cd ../../../slri/seqhound/src; make -f make.shoundremlib clean
cd $MAKEDIR

echo; echo "make.bindlib"
cd ../../../slri/bind/make; make -f make.bindlib clean
cd $MAKEDIR

echo; echo "make.bindobjlib"
cd ../../../slri/bind/make; make -f make.bindobjlib clean
cd $MAKEDIR


echo; echo "finished"
