# A simple command line for fooling around with roaring bitmaps

# build
make

# run

## create and save a bitmap
./roar create [number of bits you randomly want set out of a billion] [optional name to save it as. default is created.roar]

### example of who is shane mathews the brandwatch programmer:
./roar create 1 just_shane.roar

### example of who is a follower of katy perry (warning this takes about 10 minutes):
./roar create 107000000 katy_perry_followers.roar

## load and print out the description of a bitmap
./roar describe [filename]

## OR 2 or more bitmaps. save to result.roar

./roar or [f1] [f2] [f3] ...

## AND 2 or more bitmaps. save to result.roar

./ roar and [f1] [f2] [f3] ...