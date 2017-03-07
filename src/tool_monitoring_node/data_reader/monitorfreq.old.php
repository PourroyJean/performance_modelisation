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

$data_monitorfreq = `./monitorfreq $last_timer_known  $dire_to_use/turbostats.*`;
$data_monitormembw = `./monitormembw $last_timer_known  $dire_to_use/membwstats.*`;
$data_monitorpower = `./monitorpower $last_timer_known  $dire_to_use/cpupowerstats.*`;



$data_monitorfreq = json_decode($data_monitorfreq);
$data_monitorpower = json_decode($data_monitorpower);
$data_monitormembw = json_decode($data_monitormembw);


if( $data_monitorfreq != "" && $data_monitorpower != "" && $data_monitormembw != "") {

    $index_monitorpower = count($data_monitorpower)-1;
    $index_monitorfreq  = count($data_monitorfreq)-1;
    $index_monitormembw  = count($data_monitormembw)-1;

    $i = 0;
    while ( isset($data_monitorpower[$index_monitorpower]->timer) &&
            isset($data_monitormembw[$index_monitormembw]->timer) && 
            $index_monitorfreq >= 0 && $i<60 ) {
        
        $data_monitorfreq[$index_monitorfreq]->data_power_temp = $data_monitorpower[$index_monitorpower]->data_power_temp;
        $data_monitorfreq[$index_monitorfreq]->data_membw = $data_monitormembw[$index_monitormembw]->data_membw;
        $index_monitorfreq--;
        $index_monitorpower--;
        $index_monitormembw--;
        $i++;
    }

    echo json_encode($data_monitorfreq);
}

?>
