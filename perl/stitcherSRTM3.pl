#!/usr/bin/perl -w

# this merges SRTM 90m v4.1 (5x5 tiles) from
# http://www.cgiar-csi.org/data/srtm-90m-digital-elevation-database-v4-1
# into a single large file

# download all tiles of a rectangular area and adapt $lat and $lon ranges in loops below accordingly

# be aware, this is damn slow, but works


for ($lat = 1; $lat <= 5; $lat++) {

  for ($row = 0; $row < 6000; $row++){

    for ($lon = 35; $lon <= 42; $lon++) {

      $skip=48350;
      $skip += $row*(6001*2);
      system ("dd if=srtm_${lon}_0${lat}.tif  oflag=append conv=notrunc skip=$skip count=12000 bs=1 of=srtm3-v4.1.raw > /dev/null 2>&1  ");
      #print "dd if=srtm_${lon}_0${lat}.tif oflag=append  skip=$skip count=12000 bs=1 of=srtm3.raw \n" ;

    }

    print "$lat  $row / 6000 \n" if ($row % 100 == 0);
  }
}
