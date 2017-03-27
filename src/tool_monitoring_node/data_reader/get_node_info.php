<?php

if(isset($_POST['directory'])) {
    $dire_to_use = $_POST['directory'];
} else {
    $dire_to_use = "data";
}

// Number of core in the node
$cmd = "ls $dire_to_use/turbostats.* | wc -w";
$nb_core =  `$cmd`;
$info_to_send['nb_core'] = $nb_core;

// Number of socket in the node
$nb_socket = 1;
while(`ls -f ../data/turbostats.$nb_socket.* 2>/dev/null | wc -w`!= 0 ){
    $nb_socket++;
}
$info_to_send['nb_socket'] = $nb_socket;



echo json_encode($data_per_core_to_send);



?>
