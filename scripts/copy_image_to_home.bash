############################################################################
# Some simple setup variables
############################################################################
set TAG=""
set STORE="/home/${USER}/BOOT_XORIF"
set BASEPATH="../output"
set BINPATH="./images/linux"
set CREATIONTIME=`date --utc +%Y%m%d_%H%M%S`

############################################################################
# Alow TAG modification through command line
############################################################################
if ( $1 == "" ) then
   echo "No build Given, Exiting chose one of:"
   ls -la $BASEPATH
   exit
else 
   echo "Setting build to $1"
   set TAG=$1
endif

############################################################################
# Alow STORE modification through command line
############################################################################
if ( $2 != "" ) then
   echo "Setting $STORE to $2"
   set STORE=$2
endif

############################################################################
# Some simple setup variables
############################################################################
set DEST=$STORE/${TAG}_${CREATIONTIME}
set PL_SRC="$BASEPATH/${TAG}/petalinux/images/linux"

set PL_BOOTB=${PL_SRC}/BOOT.BIN
set PL_BOOTS=${PL_SRC}/boot.scr
set PL_IMGUB=${PL_SRC}/image.ub

set VI_LTX=$BASEPATH/${TAG}/vivado/${TAG}.runs/impl_1/design_1_wrapper.ltx
set VI_LTX=$BASEPATH/${TAG}/vivado/*.runs/impl_1/*.ltx

if ( ! -e $PL_BOOTB ) then
   echo "Exiting, non-existing file $PL_BOOTB"
   exit
endif

if ( ! -e $PL_BOOTS ) then
   echo "Exiting, non-existing file $PL_BOOTS"
   exit
endif
if ( ! -e $PL_IMGUB ) then
   echo "Exiting, non-existing file $PL_IMGUB"
   exit
endif

############################################################################
# check and make your store directory
############################################################################
if ( ! -d $STORE ) then
   mkdir $STORE
else 
   echo "Store exists not recreating @ $STORE"
endif

# make a date_time tagged directory to copy files into
mkdir $DEST

echo "Copy PL files from $PL_SRC to $DEST"
cp ${PL_BOOTB} $DEST/.
cp ${PL_BOOTS} $DEST/.
cp ${PL_IMGUB} $DEST/.
cp ${VI_LTX}   $DEST/.

echo "##### CONTENT OF $DEST"
ls -la $DEST
