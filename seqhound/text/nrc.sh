$Id: nrc.sh,v 1.1 2005/02/25 21:17:26 kzheng Exp $
#1 get files
# /usr/sfw/bin/wget -P nrc -m http://ii200.iit.nrc.ca/~martinj/

#2. parse files
for i in `find . -name *.pred`
do 
    echo $i
    cat $i >> docscore.svm
    
done

# load into mysql table

