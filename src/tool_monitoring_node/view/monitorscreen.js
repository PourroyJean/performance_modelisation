
/*global $*/
var timeoutID_request_data = 0 , timeoutID_fetch_sample = 0;
var fetch_semple_is_running = false;
var number_of_empty_data_received = 0;
var nb_socket = 0;
var nb_core = 0;

/*  Finals */
var max_bw_qpi = 40;
var max_bw_pci = 15;
var max_bw_membw = 80;
var nb_pci_bus = 4;
var supress_latency = 1; // 0 = no, 1 = yes

var current_data_index;
var time_of_simulation = 0;
var last_timer_known = 0;
var current_data = 0;           // Data currently displayed
var data_queue = [];            // A queue of data. The next to be used is at the hightest index

$(document).ready(function(){
    // main
    $("#data_status").css("background-color", "#d7d7d7");
    $("#equalizer_timer_status").css("background-color", "#d7d7d7");

    $("#start_request").click(function(){   // setup the launcher button

        if (timeoutID_request_data != 0){
            $("article").prepend("<p>You are already requesting data.</p>");
            //alert("You are already requesting data.");
        } else {
            $("article").prepend("<p>directory_input = '" + $("#directory_input").val() + "'</p>");
            request_data();
            timeoutID_request_data = setInterval(function(){
                request_data();
            }, 4000);
        }
    });
    $("#stop_request").click(function(){   // setup the break button
        stop_request();
    });
    $("#displayCounter").change(function(){   // setup the checkbox button action
      if($(this).is(':checked')) {
        console.log("- box checked " + $(this).is(':checked'));
        $(".counter").removeClass("hidden_counter");
      } else {
        console.log("+ box unchecked " + $(this).is(':checked'));
        $(".counter").addClass("hidden_counter");
      }
    });
});

function stop_request(){   // setup the break button
    if (timeoutID_request_data != 0) {
        $("#data_status").text("request stopped");
        $("#data_status").css("background-color", " #d2a679");
        clearInterval(timeoutID_request_data);
        timeoutID_request_data = 0;
    }
}



function request_data(){    // Add to "data_queue" the lastest data from the server

    //$("#equalizer_timer").append("_");
    var last_data_uploaded = {};
    if (data_queue.length != 0){
        last_data_uploaded = data_queue[0];
    } else if (current_data!= 0) {
        last_data_uploaded = current_data;
    }
    if (current_data!= 0 || data_queue.length != 0) {
        var nb_row_last_data_uploaded = last_data_uploaded.length;
        last_timer_known = last_data_uploaded[nb_row_last_data_uploaded-1]["timer"];
        $("#equalizer_last_timer").text(last_timer_known);
    }
    $("#data_status").text("waiting for data...");
    $("#data_status").css("background-color", " #d2a679");

    $.post("../data_reader/monitorfreq.php",
    {
        directory: $("#directory_input").val(),
        last_timer_known: last_timer_known,         // nb_row of data
        value_length: 500                           // millisecond
    },
    function(data, status){ // callback

        $("#data_status").text("data received");
        $("#data_status").css("background-color", " #9fff80");
        $("article").prepend("<p> Status: " +  status  + " __ Data: " + data + "</p>");
        if (data != "") {
            var new_data = JSON.parse(data);

            // Display error on the page, and info in browser console
            if (typeof new_data === 'undefined') {
                $("article").prepend("<p style=\"color: red;\"> ERROR: I can't understand server data </p>");
                console.log(" ERROR: I can't understand server data");
            } else if (typeof new_data !== 'undefined' && typeof new_data.error !== 'undefined') {
                $("article").prepend("<p style=\"color: red;\"> ERROR: " +  new_data.error  + "</p>");
                console.log(" ERROR: " +  new_data.error);
            } else if (typeof new_data.info !== 'undefined'  && new_data.info != 'undefined' ){
                console.log(new_data.info);
            } else if (typeof new_data[0] !== 'undefined' && typeof new_data[0].info !== 'undefined') {
                console.log(new_data[0].info);
            }

            if(typeof new_data[0] !== 'undefined') {
                number_of_empty_data_received = 0;

                if (nb_socket==0) { // Initializing hardware information
                    nb_socket = new_data[0]["data_freq"].length;
                    nb_core =   new_data[0]["data_freq"][0].length * nb_socket;
                    generate_html();
                }
                if (last_timer_known < new_data[0]["timer"]) {
                    $("#data_status").append(", and added to queue");

                    data_queue.unshift(new_data);

                    var last_data_uploaded = data_queue[0]; // lastest data
                    var nb_row_last_data_uploaded = last_data_uploaded.length;
                    last_timer_known = last_data_uploaded[nb_row_last_data_uploaded-1]["timer"];
                    $("#equalizer_last_timer").text(last_timer_known);
                    if (fetch_semple_is_running == false){
                        launch_runner();
                    }
                }
            } else {
                number_of_empty_data_received++;
                if(number_of_empty_data_received > 4) {
                    stop_request();
                }
            }
        } else {
            number_of_empty_data_received++;
            if(number_of_empty_data_received > 4) {
                stop_request();
            }
        }
    });
}

function launch_runner(){

    if (data_queue.length !=0) {
        current_data = data_queue.pop();
        current_data_index = 0;
        update_time_of_simulation();
        var interval = (current_data[1]["timer"] - current_data[0]["timer"]);

        if (interval > 0) {
            $("article").prepend("<p>Interval : " + interval + "</p>" );
            $("#equalizer_timer_status").text("running");
            $("#equalizer_timer_status").css("background-color", "#d9ff66");

            timeoutID_fetch_sample = setInterval(function(){
                fetch_sample();

                // If there a big latency
                if(supress_latency > 0) {
                  var delta_plus_5 = Number(time_of_simulation) +5 - Number(last_timer_known);
                  if(delta_plus_5 < 0) {
                      setTimeout(fetch_sample, 250);
                  }
                }
            }, 1000 * interval);
            fetch_semple_is_running = true;
        }
    }
}

function fetch_sample(){
    if (current_data_index < current_data.length){
        update_time_of_simulation();
        update_monitor();
        current_data_index++;
    } else {    // packet is finished
        if (data_queue != null && data_queue.length > 0){   // There is a next one
            current_data = data_queue.pop();
            current_data_index = 0;

            update_time_of_simulation();
            update_monitor();
            current_data_index++;
        } else {                                            // There isn't a next one
            $("#equalizer_timer_status").text("stopped");
            $("#equalizer_timer_status").css("background-color", "#d7d7d7");
            clearInterval(timeoutID_fetch_sample); // Stop runner
            fetch_semple_is_running = false;
        }
    }


}

function update_time_of_simulation(){
    time_of_simulation = current_data[current_data_index]["timer"];    // #core = 0, and time column is 0
    $("#equalizer_timer").text(time_of_simulation);
}

function update_monitor(){
    update_power_temp();
    update_membw();
    update_equalizer();
    update_pci();
    if (nb_socket == 2) {
        update_qpi();
    }

}

function update_equalizer(){

    var blue_height;    // Average
    var black_height;   // Max
    for (var i = 0; i<nb_core/nb_socket; i++){
        blue_height  = ((current_data[current_data_index]["data_freq"][0][i][2]/2)  )*100;     // avg AT
        black_height = ((current_data[current_data_index]["data_freq"][0][i][3]/2)  )*100;    // max AT

        $("#core_" + i + "> .equalizer-stick").css("height", blue_height+"%");
        $("#core_" + i + "> .equalizer-line").css("height", black_height+"%");
    }
    if (nb_socket == 2){
        for (var i = 0; i<nb_core/nb_socket; i++){
            blue_height  = ((current_data[current_data_index]["data_freq"][1][i][2]/2)  )*100;     // avg AT
            black_height = ((current_data[current_data_index]["data_freq"][1][i][3]/2)  )*100;    // max AT

            $("#core_" + (i+ nb_core/nb_socket) + "> .equalizer-stick").css("height", blue_height+"%");
            $("#core_" + (i+ nb_core/nb_socket) + "> .equalizer-line").css("height", black_height+"%");
        }
    }

}

function update_power_temp(){

    var blue_height;    // Average
    var black_height;   // Max
    var Number_to_show; // Average

    for (var i = 0; i<nb_socket; i++){
        // =========== POWER =========== //         Let's suppose a socket will not go over 150 Watt
        blue_height  = current_data[current_data_index]["data_power_temp"][i][0]  /1.5;     // avg Power (normalized)
        Number_to_show = Math.round(current_data[current_data_index]["data_power_temp"][i][0]);        // avg Power
        black_height = current_data[current_data_index]["data_power_temp"][i][1]  /1.5;   // max Power

        $("#power_socket_" + i + " .power-stick").css("height", blue_height +"%");
        $("#power_socket_" + i + " .power-stick").text(Number_to_show);
        $("#power_socket_" + i + " .power-line" ).css("height", black_height+"%");


        // ======== TEMPERATURE ======== //         Let's suppose a socket will not go over 100 °C
        blue_height  = current_data[current_data_index]["data_power_temp"][i][2];     // avg Temperature
        Number_to_show = Math.round(current_data[current_data_index]["data_power_temp"][i][2]);  // avg Temperature
        black_height = current_data[current_data_index]["data_power_temp"][i][3];   // max Temperature

        $("#temp_socket_" + i + " .temp-stick").css("height", blue_height +"%");
        $("#temp_socket_" + i + " .temp-stick").text(Number_to_show);
        $("#temp_socket_" + i + " .temp-line" ).css("height", black_height+"%");
    }

}



function update_membw(){


    var blue_height;    // Average
    var black_height;   // Max
    var Number_to_show_avg; // Average
    var Number_to_show_max; // Max
    var Number_to_show_max_both_way; // Sum of both max
    var last_height = 0;
    for (var i = 0; i<nb_socket; i++){

        var sum_debit_max = 0; // GB/s
        var up_down;
        for (var j = 0; j<2; j++){
            if(j == 0) {
                up_down = "down";
                Number_to_show_avg = current_data[current_data_index]["data_membw"][i][0]/1000;        // avg GB/s
                Number_to_show_max = current_data[current_data_index]["data_membw"][i][1]/1000;        // max GB/s
            } else {
                up_down = "up"
                Number_to_show_avg = current_data[current_data_index]["data_membw"][i][2]/1000;        // avg GB/s
                Number_to_show_max = current_data[current_data_index]["data_membw"][i][3]/1000;        // max GB/s
            }
            Number_to_show_max_both_way = current_data[current_data_index]["data_membw"][i][4]/1000;        // max GB/s

            blue_height   = Number_to_show_avg*(100/max_bw_membw);     // avg bandwith (normalized to max_bw_membw GB/s max)
            black_height  = Number_to_show_max*(100/max_bw_membw);    // max bandwith  (normalized to max_bw_membw GB/s max)


            $("#membw_socket_" + i + " .membw-counter-avg-" + up_down + " .number-membw").text(parseFloat(Number_to_show_avg).toFixed(1));
            $("#membw_socket_" + i + " .membw-counter-max-" + up_down + " .number-membw").text(parseFloat(Number_to_show_max).toFixed(1));
            $("#membw_socket_" + i + " .membw-counter-max-sum .number-membw").text(parseFloat(Number_to_show_max_both_way).toFixed(1));
            $("#membw_socket_" + i + " .membw-arrow-container .membw-stick-container .membw-stick-" + up_down).css("height", (100-blue_height) +"%");
            $("#membw_socket_" + i + " .membw-arrow-container .membw-stick-container .membw-line-"  + up_down).css("height", black_height +"%");

        }

    }

}

function update_qpi(){

    if(current_data[current_data_index]["data_qpi"] != undefined ) {
      var blue_height;    // Average
      var black_height;   // Max
      var Number_to_show_avg; // Average
      var Number_to_show_max; // Max
      var left_right ;
      for (var i = 0; i<2; i++){
          if(i == 0) {
              left_right = "right";
          } else {
              left_right = "left"
          }

          Number_to_show_avg = Math.round((current_data[current_data_index]["data_qpi"][i][0])/100 )/10;        // avg GB/s
          Number_to_show_max = Math.round((current_data[current_data_index]["data_qpi"][i][1])/100 )/10;        // max GB/s
          blue_height   = 100 - Number_to_show_avg*(100/max_bw_qpi);     // avg bandwith (normalized to max_bw_qpi GB/s max)
          black_height  = Number_to_show_max*(100/max_bw_qpi);    // max bandwith  (normalized to max_bw_qpi GB/s max)


          $(".qpi-counter-avg-" + left_right + " .number-qpi").text(parseFloat(Number_to_show_avg).toFixed(1));
          $(".qpi-counter-max-" + left_right + " .number-qpi").text(parseFloat(Number_to_show_max).toFixed(1));
          $(".qpi-stick-" + left_right).css("width", blue_height +"%");
          $(".qpi-line-"  + left_right).css("width", black_height+"%");
      }
    }
}


function update_pci(){

    var blue_height;    // Average
    var black_height;   // Max
    var Number_to_show_avg_down, Number_to_show_avg_up; // Average
    var Number_to_show_max_down, Number_to_show_max_up; // Max
    for (var i = 0; i<nb_socket; i++){
      for (var bus = 0; bus<nb_pci_bus; bus++){


        Number_to_show_avg_up   = Math.round((current_data[current_data_index]["data_pci"][i][bus][0])/100 )/10;        // avg GB/s
        Number_to_show_max_up   = Math.round((current_data[current_data_index]["data_pci"][i][bus][1])/100 )/10;        // max GB/s
        Number_to_show_avg_down = Math.round((current_data[current_data_index]["data_pci"][i][bus][2])/100 )/10;        // avg GB/s
        Number_to_show_max_down = Math.round((current_data[current_data_index]["data_pci"][i][bus][3])/100 )/10;        // max GB/s

        blue_height_up     = 100 - Number_to_show_avg_up*(100/max_bw_pci);     // avg bandwith (normalized to max_bw_pci GB/s max)
        black_height_up    = Number_to_show_max_up*(100/max_bw_pci);           // max bandwith (normalized to max_bw_pci GB/s max)
        blue_height_down   = 100 - Number_to_show_avg_down*(100/max_bw_pci);   // avg bandwith (normalized to max_bw_pci GB/s max)
        black_height_down  = Number_to_show_max_down*(100/max_bw_pci);         // max bandwith (normalized to max_bw_pci GB/s max)

        if(bus == 1) {
          // 4 lanes instead of 16, so 4x less bw
          blue_height_up     = 100 - 4 * Number_to_show_avg_up*(100/max_bw_pci);     // avg bandwith (normalized to max_bw_pci GB/s max)
          black_height_up    = Number_to_show_max_up*(100/max_bw_pci)*4;           // max bandwith (normalized to max_bw_pci GB/s max)
          blue_height_down   = 100 - 4 * Number_to_show_avg_down*(100/max_bw_pci);   // avg bandwith (normalized to max_bw_pci GB/s max)
          black_height_down  = Number_to_show_max_down*(100/max_bw_pci)*4;         // max bandwith (normalized to max_bw_pci GB/s max)
        }

        $("#pci_socket_"+i+" .pci-counter-hover-bus-" + bus + " > .pci-counter-avg-up > .number-pci").text(parseFloat(Number_to_show_avg_up).toFixed(1));
        $("#pci_socket_"+i+" .pci-counter-hover-bus-" + bus + " > .pci-counter-max-up > .number-pci").text(parseFloat(Number_to_show_max_up).toFixed(1));
        $("#pci_socket_"+i+" .bus-number-" + bus + " > .pci-stick-container-up > .pci-stick-up").css("height", blue_height_up +"%");
        $("#pci_socket_"+i+" .bus-number-" + bus + " > .pci-stick-container-up > .pci-line-up").css("height", black_height_up+"%");
        $("#pci_socket_"+i+" .pci-counter-hover-bus-" + bus + " > .pci-counter-avg-down > .number-pci").text(parseFloat(Number_to_show_avg_down).toFixed(1));
        $("#pci_socket_"+i+" .pci-counter-hover-bus-" + bus + " > .pci-counter-max-down > .number-pci").text(parseFloat(Number_to_show_max_down).toFixed(1));
        $("#pci_socket_"+i+" .bus-number-" + bus + " > .pci-stick-container-down > .pci-stick-down").css("height", blue_height_down +"%");
        $("#pci_socket_"+i+" .bus-number-" + bus + " > .pci-stick-container-down > .pci-line-down").css("height", black_height_down+"%");
      }
    }

}

function generate_html(){

   for (var i = 0; i < nb_socket; i++) {
        create_arrow_membw_html($("#Monitoring-screen"), i);
   }
   for (var i = 0; i < nb_socket; i++) {
        if(i==1) {
            create_qpi_html($("#Monitoring-screen"));
        }
        create_socket_html($("#Monitoring-screen"), i);
   }
   create_pci_html($("#Monitoring-screen"));

}

function create_qpi_html(parent){
    var arrow = "";
    arrow += "<div class=\"qpi-container\">";

    arrow +=   "<div class=\"qpi-counter-max-right counter\"><div class=\"title-qpi\">Max </div><div class=\"number-qpi\"> </div><div class=\"unit-qpi\">GB/s </div> </div>";
    arrow +=   "<div class=\"qpi-counter-avg-right counter\"><div class=\"title-qpi\">Avg </div><div class=\"number-qpi\"> </div><div class=\"unit-qpi\">GB/s </div> </div>";
    arrow +=   "<div class=\"qpi-counter-max-left counter\"> <div class=\"title-qpi\">Max </div><div class=\"number-qpi\"> </div><div class=\"unit-qpi\">GB/s </div> </div>";
    arrow +=   "<div class=\"qpi-counter-avg-left counter\"> <div class=\"title-qpi\">Avg </div><div class=\"number-qpi\"> </div><div class=\"unit-qpi\">GB/s </div> </div>";

    arrow +=    "<div class=\"qpi-arrow-container\">";
    arrow +=      "<img src=\" pictures/white-arrow-right.png\" alt=\"arrow\" class=\"icon-qpi-arrow-right\" >";
    arrow +=      "<img src=\" pictures/white-arrow-left.png\"   alt=\"arrow\" class=\"icon-qpi-arrow-left\" >";
    arrow +=      "<img src=\" pictures/switch.png\" alt=\"Switch\" class=\"icon-switch\" >";
    arrow +=      "<div class=\"qpi-stick-container qpi-stick-container-right\"> ";
    arrow +=        "<div class=\"qpi-echelle\"> ";
    for(var i = 0; i<4; i++) {
          arrow +=    "<div class=\"qpi-echelle-number-" + i + "\"> </div> ";
    }
    arrow +=        "</div>";
    arrow +=        "<div class=\"qpi-stick qpi-stick-right\"> </div> ";
    arrow +=        "<div class=\"qpi-line qpi-line-right\"  > </div> ";

    arrow +=      "</div>";
    arrow +=      "<div class=\"qpi-stick-container qpi-stick-container-left   \">";
    arrow +=        "<div class=\"qpi-echelle\"> ";
    for(var i = 0; i<4; i++) {
          arrow +=    "<div class=\"qpi-echelle-number-" + i + "\"> </div> ";
    }
    arrow +=        "</div>";
    arrow +=        "<div class=\"qpi-stick qpi-stick-left\"> </div> ";
    arrow +=        "<div class=\"qpi-line qpi-line-left\"  > </div> ";
    arrow +=      "</div>";
    arrow +=    "</div>";

    parent.append(arrow);
    for (var i = 0; i<4; i++) {
        if(i%2 == 0) {
            $(".qpi-echelle .qpi-echelle-number-"+i).css("background-color", "#99f7ff");
        } else {
            $(".qpi-echelle .qpi-echelle-number-"+i).css("background-color", "#36a2eb");
        }
        //$(".qpi-stick-container-right .qpi-echelle .qpi-echelle-number-"+i).css("background-color", "black");
        $(".qpi-stick-container-right .qpi-echelle .qpi-echelle-number-"+i).css("left", " " + (i*(100/4))+"%");
        $(".qpi-stick-container-left .qpi-echelle .qpi-echelle-number-"+i).css("right", " " + (i*(100/4))+"%");
    }


}

function create_arrow_membw_html(parent, socket_number){
    var arrow = "";
    arrow += "<div class=\"membw-container\" id=\"membw_socket_" + socket_number + "\">";
    arrow +=   "<div class=\"membw-arrow-container\">";
    arrow +=     "<img src=\" pictures/white-arrow-down.png\" alt=\"membw\" class=\"icon-membw-arrow-down\" >";
    arrow +=     "<img src=\" pictures/white-arrow-up.png\" alt=\"membw\" class=\"icon-membw-arrow-up\" >";
    arrow +=     "<img src=\" pictures/white-ram.png\" alt=\"RAM\" class=\"icon-ram\" >";
    arrow +=     "<div class=\"membw-stick-container membw-stick-container-down\"> ";

    arrow +=        "<div class=\"membw-echelle\"> ";
    for(var i = 0; i<8; i++) {
          arrow +=    "<div class=\"membw-echelle-number-" + i + "\"> </div> ";
    }
    arrow +=        "</div>";
    arrow +=        "<div class=\"membw-stick membw-stick-down\"> </div> ";
    arrow +=        "<div class=\"membw-line membw-line-down\"  > </div> ";


    arrow +=     "</div>";
    arrow +=     "<div class=\"membw-stick-container membw-stick-container-up\"> ";
    arrow +=       "<div class=\"membw-echelle\"> ";
    for(var i = 0; i<8; i++) {
          arrow +=   "<div class=\"membw-echelle-number-" + i + "\"> </div> ";
    }
    arrow +=       "</div>";
    arrow +=       "<div class=\"membw-stick membw-stick-up\"> </div> ";
    arrow +=       "<div class=\"membw-line membw-line-up\"  > </div> ";
    arrow +=     "</div>";
    arrow +=   "</div>";
    arrow +=   "<div class=\"membw-counter-max-down counter\"><div class=\"title-membw\">Max </div><div class=\"number-membw\"> </div><div class=\"unit-membw\">GB/s </div> </div>";
    arrow +=   "<div class=\"membw-counter-avg-down counter\"><div class=\"title-membw\">Avg</div><div class=\"number-membw\"> </div><div class=\"unit-membw\">GB/s </div> </div>";
    arrow +=   "<div class=\"membw-counter-max-up counter\"><div class=\"title-membw\">Max</div><div class=\"number-membw\"> </div><div class=\"unit-membw\">GB/s </div> </div>";
    arrow +=   "<div class=\"membw-counter-avg-up counter\"><div class=\"title-membw\">Avg</div><div class=\"number-membw\"> </div><div class=\"unit-membw\">GB/s </div> </div>";
    arrow +=   "<div class=\"membw-counter-max-sum counter\"> <div class=\"title-membw\">Max </div><div class=\"number-membw\"> </div><div class=\"unit-membw\">GB/s </div> </div>";
    arrow += "</div>";
    parent.append(arrow);


    for (var i = 0; i<8; i++) {
        if(i%2 == 0) {
            $(".membw-echelle .membw-echelle-number-"+i).css("background-color", "#99f7ff");
        } else {
            $(".membw-echelle .membw-echelle-number-"+i).css("background-color", "#36a2eb");
        }
        //$(".membw-stick-container-right .membw-echelle .membw-echelle-number-"+i).css("background-color", "black");
        $(".membw-stick-container-down .membw-echelle .membw-echelle-number-"+i).css( "top", " " + (i*(100/8))+"%");
        $(".membw-stick-container-up .membw-echelle .membw-echelle-number-"+i).css("bottom", " " + (i*(100/8))+"%");
    }

}

function create_socket_html(parent, socket_number){
    parent.append("<div class=\"socket\" id=\"socket_" + socket_number + "\"> </div>");

    create_jauge_temp($("#socket_"+ socket_number), socket_number);
    create_jauge_power($("#socket_"+ socket_number), socket_number);

    create_equalizer_html($("#socket_"+ socket_number), socket_number );
}

function create_jauge_temp(parent, socket_number){
    var stick = "";
    stick += "<div class=\"temp-stick-container\" id=\"temp_socket_" + socket_number + "\">";
    stick +=     "<div class=\"temp-stick\"> 30 </div> ";
    stick +=     "<div class=\"temp-line\" > </div> ";
    stick += "</div>";
    parent.append(stick);

    var title_temperature = "<img  src=\" pictures/temperature.png\" alt=\"Temperature\" class=\"icon_temperature\" >";
    parent.append(title_temperature);
}

function create_jauge_power(parent, socket_number){
    var stick = "";
    stick += "<div class=\"power-stick-container\" id=\"power_socket_" + socket_number + "\">";
    stick +=     "<div class=\"power-stick\"> 30 </div> ";
    stick +=     "<div class=\"power-line\" > </div> ";
    stick += "</div>";
    parent.append(stick);

    var title_power ="<img  src=\" pictures/power.png\" alt=\"Power\" class=\"icon_power\" >";
    parent.append(title_power);
}


function create_equalizer_html(parent, socket_number){
    var equalizer = "<div class=\"equalizer\">"
    var scale;

    scale = "<div id=\"echelle\">";
    scale +=    "<div id=\"echelle-1_5\"></div>";
    scale +=    "<div id=\"echelle-1_0\"></div>";
    scale +=    "<div id=\"echelle-0_5\"></div>";
    scale +=    "<div id=\"echelle-0_0\"></div>";
    scale += "</div>";


    var str = equalizer + scale + "</div>";
    $("#socket_"+ socket_number).append(str);
    $("#socket_"+ socket_number).append("<span class=\"echelle_indicateur-0_0 indicateur\"> 0% </span> ");
    $("#socket_"+ socket_number).append("<span class=\"echelle_indicateur-0_5 indicateur\"> 50% </span> ");
    $("#socket_"+ socket_number).append("<span class=\"echelle_indicateur-1_0 indicateur\"> 100% </span> ");
    $("#socket_"+ socket_number).append("<span class=\"echelle_indicateur-1_5 indicateur\"> 150% </span> ");
    $("#socket_"+ socket_number).append("<span class=\"echelle_indicateur-2_0 indicateur\"> 200% </span> ");


    var last_equalizer=$("#socket_"+ socket_number +" .equalizer");


    for(var i = 0; i<nb_core/nb_socket; i++){
        create_corestick_html(last_equalizer, i + (socket_number*(nb_core/2)) );
    }



    if (socket_number == 1) {
        //$(".socket").css("width", 80+"%");
        //$("#Monitoring-screen #socket_0").css("margin-right", 18 + "%");
    }

    var width_stick = (100-(nb_core/nb_socket)/2) / (nb_core/nb_socket);
    $(".equalizer-stick-container").css("width", width_stick+"%");

    var title_proc ="<img  src=\" pictures/processor.png\" alt=\"Processor\" class=\"icon_proc\" >";
    parent.append(title_proc);

}

function create_corestick_html(parent, core_number){

    var stick = "";
    stick += "<div class=\"equalizer-stick-container\" id=\"core_" + core_number + "\">";
    stick += "<div class=\"equalizer-stick\"> </div> ";
    stick += "<div class=\"equalizer-line\" > </div> ";
    stick += "</div>";

    parent.append(stick);

}

function create_pci_html(parent) {

    var pci_container = "";
    pci_container += "<div class=\"pci\">";

    for(var socket_number = 0; socket_number<nb_socket; socket_number++){
        pci_container +=  "<div class=\"pci-container\" id=\"pci_socket_" + socket_number + "\">";
        for(var bus_number = 0; bus_number < nb_pci_bus; bus_number++) {
            pci_container +=  "<div class=\"pci-container-bus bus-number-" + bus_number + "\">";

            var up_down = ["up", "down"];
            for (var side in up_down ) {
              pci_container +=     "<img src=\"pictures/new-white-arrow-"+up_down[side]+".png\"   alt=\"pci-e\" class=\"icon-pcie-arrow-"+up_down[side]+" icon-pcie-arrow\" >";
              pci_container +=     "<div class=\"pci-stick-container pci-stick-container-"+up_down[side]+"\"> ";
              pci_container +=        "<div class=\"pci-echelle\"> ";
              for(var i = 0; i<2; i++) {
                    pci_container +=    "<div class=\"pci-echelle-number-" + i + "\"> </div> ";
              }
              pci_container +=        "</div>"; //class = "pci-echelle"
              pci_container +=        "<div class=\"pci-stick pci-stick-"+up_down[side]+"\"> </div> ";
              pci_container +=        "<div class=\"pci-line pci-line-"+up_down[side]+"\"  > </div> ";
              pci_container +=      "</div>"; //class = "pci-stick-container"
            }
            pci_container +=        "<div class=\"pci-counter-hover pci-counter-hover-bus-"+bus_number+"\"> ";
            pci_container +=          "<div class=\"pci-counter-max-up pci-counter\">"
            pci_container +=            "<div class=\"title-pci\">Max↑ </div><div class=\"number-pci\"> </div><div class=\"unit-pci\">GB/s </div>";
            pci_container +=          "</div>";
            pci_container +=          "<div class=\"pci-counter-avg-up pci-counter\">"
            pci_container +=            "<div class=\"title-pci\">Avg↑ </div><div class=\"number-pci\"> </div><div class=\"unit-pci\">GB/s </div>";
            pci_container +=          "</div>";
            pci_container +=          "<div class=\"pci-counter-max-down pci-counter\">"
            pci_container +=            "<div class=\"title-pci\">Max↓ </div><div class=\"number-pci\"> </div><div class=\"unit-pci\">GB/s </div>";
            pci_container +=          "</div>";
            pci_container +=          "<div class=\"pci-counter-avg-down pci-counter\">"
            pci_container +=            "<div class=\"title-pci\">Avg↓ </div><div class=\"number-pci\"> </div><div class=\"unit-pci\">GB/s </div>";
            pci_container +=          "</div>";
            pci_container +=        "</div>"; //class = "pci-counter-hover"

            pci_container += "</div>"; //class = "pci-container-bus"
        }
        pci_container += "</div>"; //class = "pci-container"
    }

    pci_container += "</div>";  //class = "pci"

    parent.append(pci_container);


    for (var i = 0; i<2; i++) {
        //$(".qpi-stick-container-right .qpi-echelle .qpi-echelle-number-"+i).css("background-color", "black");
        $(".pci-stick-container-up .pci-echelle .pci-echelle-number-"+i).css("bottom", " " + (i*(100/2))+"%");
        $(".pci-stick-container-down .pci-echelle .pci-echelle-number-"+i).css("top", " " + (i*(100/2))+"%");
    }

}
