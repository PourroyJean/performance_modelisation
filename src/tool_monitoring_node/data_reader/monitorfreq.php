<?php
error_reporting(-1);
if(isset($_POST['directory'])) {        // TODO : Verify files name (security)
    $dire_to_use = $_POST['directory'];
} else {
    $dire_to_use = "../data";
}

if(isset($_POST['value_length'])) {         // millisecond
    $value_length = $_POST['value_length'];
} else {
    $value_length = 500;
}

$nb_iter = $value_length/10;                // data is "per 10 millisecond"

if ( isset($_POST['last_timer_known'])){
    $last_timer_known = $_POST['last_timer_known'];
} else {
    $last_timer_known = 0;
}

$number_of_cpu_file   =  `ls $dire_to_use/turbostats.* | wc -l`;
$number_of_power_file =  `ls $dire_to_use/cpupowerstats.* | wc -l`;
$number_of_membw_file =  `ls $dire_to_use/membwstats.* | wc -l`;
$number_of_qpi_file   =  `ls $dire_to_use/qpi*stats.* | wc -l`;
$number_of_pci_file   =  `ls $dire_to_use/pci*stats.* | wc -l`;
if ($number_of_cpu_file * $number_of_power_file * $number_of_membw_file != 0) {

    $data_monitorfreq = `./monitorfreq $last_timer_known  $dire_to_use/turbostats.*`;
    $data_monitorpower = `./monitorpower $last_timer_known  $dire_to_use/cpupowerstats.*`;
    $data_monitormembw = `./monitormembw $last_timer_known  $dire_to_use/membwstats.*`;
    $data_monitorqpi = `./monitorqpi $last_timer_known  $dire_to_use/qpi*stats.*` ;
    $data_monitorpci = `./monitorpci $last_timer_known  $dire_to_use/pci*stats.*` ;

    $data_monitorfreq = json_decode($data_monitorfreq);
    $data_monitorpower = json_decode($data_monitorpower);
    $data_monitormembw = json_decode($data_monitormembw);
    if($number_of_qpi_file == 0) {
      $dual_socket = false;
    } else {
      $dual_socket = true;
      $data_monitorqpi = json_decode($data_monitorqpi);
    }
    if($number_of_pci_file != 0) {
      $data_monitorpci = json_decode($data_monitorpci);
    }

    if( $data_monitorfreq != "" && $data_monitorpower != "" && $data_monitormembw != "" && ($data_monitorqpi != "" || !$dual_socket)) {

        $first_freq_value_sent = $data_monitorfreq[0]->timer ;
        $first_power_value_sent = $data_monitorpower[0]->timer ;
        $first_membw_value_sent = $data_monitormembw[0]->timer ;
        if($dual_socket) {
          $first_qpi_value_sent = $data_monitorqpi[0]->timer ;
        }

        $index_monitorpower = count($data_monitorpower)-1;
        $index_monitorfreq  = count($data_monitorfreq)-1;
        $index_monitormembw  = count($data_monitormembw)-1;
        if($dual_socket) {
          $index_monitorqpi  = count($data_monitorqpi)-1;
        }
        $index_monitorpci  = count($data_monitorpci)-1;

        while ( (    $data_monitorfreq[$index_monitorfreq ]->timer  != $data_monitorpower[$index_monitorpower]->timer  ||
                     $data_monitorpower[$index_monitorpower]->timer != $data_monitormembw[$index_monitormembw]->timer  ||
                     $data_monitormembw[$index_monitormembw]->timer != $data_monitorpci[$index_monitorpci]->timer  ||
    (!$dual_socket || $data_monitorpci[$index_monitorpci]->timer != $data_monitorqpi[  $index_monitorqpi ]->timer)) &&
                $index_monitorfreq > 0 && $index_monitorpower > 0 && $index_monitormembw > 0 && $index_monitorpci > 0 &&  ($dual_socket && $index_monitorqpi > 0)) {

            if($data_monitorfreq[$index_monitorfreq]->timer > $data_monitorpower[$index_monitorpower]->timer) {
                array_splice($data_monitorfreq, $index_monitorfreq, $index_monitorfreq);
                $index_monitorfreq--;
            }
            if($data_monitorpower[$index_monitorpower]->timer > $data_monitormembw[$index_monitormembw]->timer) {
                $index_monitorpower--;
            }
            if($data_monitormembw[$index_monitormembw]->timer > $data_monitorpci[$index_monitorpci]->timer) {
                $index_monitormembw--;
            }

            if($dual_socket) {
              if($data_monitorpci[$index_monitorpci]->timer > $data_monitorqpi[$index_monitorqpi]->timer) {
                  $index_monitorpci--;
              }
              if($data_monitorqpi[$index_monitorqpi]->timer > $data_monitorfreq[$index_monitorfreq]->timer) {
                  $index_monitorqpi--;
              }
            } else {
              if($data_monitorpci[$index_monitorpci]->timer > $data_monitorfreq[$index_monitorfreq]->timer) {
                  $index_monitorpci--;
              }
            }

        }

        while (     isset($data_monitorfreq[ $index_monitorfreq ]->timer) &&
                    isset($data_monitorpower[$index_monitorpower]->timer) &&
                    isset($data_monitormembw[$index_monitormembw]->timer) &&
                    isset($data_monitorpci[  $index_monitorpci  ]->timer) &&
    (!$dual_socket|| isset($data_monitorqpi[ $index_monitorqpi  ]->timer)) ) {


            $data_monitorfreq[$index_monitorfreq]->data_power_temp  = $data_monitorpower[$index_monitorpower]->data_power_temp;
            $data_monitorfreq[$index_monitorfreq]->data_membw       = $data_monitormembw[$index_monitormembw]->data_membw;
            $data_monitorfreq[$index_monitorfreq]->data_pci         = $data_monitorpci[$index_monitorpci]->data_pci;
            if($dual_socket) {
              $data_monitorfreq[$index_monitorfreq]->data_qpi       = $data_monitorqpi[  $index_monitorqpi  ]->data_qpi;
            }
            $index_monitorfreq--;
            $index_monitorpower--;
            $index_monitormembw--;
            $index_monitorpci--;
            $index_monitorqpi--;
        }
        array_splice($data_monitorfreq, 0, $index_monitorfreq+1);

        echo json_encode($data_monitorfreq);
    } else { // One of the object has no data to give
        $info = "These object didn't give data since $last_timer_known sec : ";
        if ($data_monitorfreq == "") {
            $info = "$info monitorfreq   ";
        }
        if ($data_monitorpower == "") {
            $info = "$info monitorpower   ";
        }
        if ($data_monitormembw  == "") {
            $info = "$info monitormembw   ";
        }
        if ($data_monitorpci  == "") {
            $info = "$info monitorpci   ";
        }
        if ($data_monitorqpi == "" && $dual_socket) {
            $info = "$info monitorqpi   ";
        }

        $anwser->info = $info;
        echo json_encode($anwser);


    }
} else { // There is an error
     $anwser = new stdClass();
     $error = "Undefined error.";
     if($number_of_cpu_file + $number_of_power_file + $number_of_membw_file + $number_of_qpi_file + $number_of_pci_file == 0) {
         $error = "No file found";
     } else {
         $error = " Files are missing : CPU=$number_of_cpu_file   POWER=$number_of_power_file   MEMBW=$number_of_membw_file   PCI=$number_of_pci_file   QPI=$number_of_qpi_file";
     }


     $anwser->error = $error;
     echo json_encode($anwser);
}

?>
