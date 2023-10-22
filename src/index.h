const char HTML_MAIN[] PROGMEM = R"rawliteral(

<figure class="text-center"><h2><b>ALIS FVE</b></h2></figure>
<div class="border border-dark">
<figure class="text-left"><h2 style="font-size:18px;"><b>Data EPEVER</b></h2></figure>
<div class="row gx-0 mb-2">
<div class="col-4">
<div class="border border-dark">
<div class="bg-primary text-white">FVE: </div>
</div> </div>
<div class="col">
<div class="border border-dark">
<div class="bg-secondary text-white"><span id="solarV" >No Data</span>&nbsp;&nbsp;<span id="solarA" ></span>&nbsp;&nbsp;<span id="solarW" ></span></div>
</div></div></div>
<div class="row gx-0 mb-2">
<div class="col-4">
<div class="border border-dark">
<div class="bg-primary text-white">Baterie: </div>
</div> </div>
<div class="col">
<div class="border border-dark">
<div class="bg-secondary text-white"><span id="batV">No Data</span>&nbsp;&nbsp;<span id="batA"></span>&nbsp;&nbsp;<span id="batW" ></span></div>
</div>
</div></div>
<div class="row gx-0 mb-2">
<div class="col-4">
<div class="border border-dark">
<div class="bg-primary text-white">DC Zátěž: </div>
</div></div>
<div class="col">
<div class="border border-dark">
<div class="bg-secondary text-white"><span id="dcoutV">No Data</span>&nbsp;&nbsp;<span id="dcoutA"></span>&nbsp;&nbsp;<span id="dcoutW" ></span></div>
</div>
</div></div>
<div class="row gx-0 mb-2">
<div class="col-4">
<div class="border border-dark">
<div class="bg-primary text-white">AC Napětí: </div>
</div></div>
<div class="col">
<div class="border border-dark">
<div class="bg-secondary text-white"><span id="acoutV">No Data</span></div>
</div>
</div></div>
<div class="row gx-0 mb-2">
<div class="col-4">
<div class="border border-dark">
<div class="bg-primary text-white">AC Zátěž: </div>
</div></div>
<div class="col">
<div class="border border-dark">
<div class="bg-secondary text-white"><span id="acoutW">No Data</span></div>
</div>
</div></div>
<div class="row gx-0 mb-2">
<div class="col-4">
<div class="border border-dark">
<div class="bg-primary text-white">Teplota MPPT: </div>
</div></div>
<div class="col">
<div class="border border-dark">
<div class="bg-secondary text-white"><span id="mpptTemp">No Data</span></div>
</div>
</div>
</div>
<div class="row gx-0 mb-2">
<div class="col-4">
<div class="border border-dark">
<div class="bg-primary text-white">Vyrobeno Dnes: </div>
</div></div>
<div class="col">
<div class="border border-dark">
<div class="bg-secondary text-white"><span id="energyDay">No Data</span></div>
</div>
</div>
</div>
<div class="row gx-0 mb-2">
<div class="col-4">
<div class="border border-dark">
<div class="bg-primary text-white">Vyrobeno Celkem: </div>
</div></div>
<div class="col">
<div class="border border-dark">
<div class="bg-secondary text-white"><span id="energyTotal">No Data</span></div>
</div>
</div>
</div>
</div>
<div <p></p> </div>
<div class="border border-dark">
<figure class="text-left"><h2 style="font-size:18px;"><b>Data JBD BMS</b></h2></figure>

<div class="row gx-0 mb-2">
<div class="col-4">
<div class="border border-dark">
<div class="bg-primary text-white">Napětí | Proud: </div>
</div></div>
<div class="col">
<div class="border border-dark">
<div class="bg-secondary text-white"><span id="bmsV">No Data</span>&nbsp;&nbsp;<span id="bmsA"></span></div>
</div>
</div></div>
<div class="row gx-0 mb-2">
<div class="col-4">
<div class="border border-dark">
<div class="bg-primary text-white">SOC | Kapacita: </div>
</div></div>
<div class="col">
<div class="border border-dark">
<div class="bg-secondary text-white"><span id="bmsSOC">No Data</span>&nbsp;&nbsp;<span id="bmsCap"></span></div>
</div>
</div></div>
<div class="row gx-0 mb-2">
<div class="col-4">
<div class="border border-dark">
<div class="bg-primary text-white">Cykly: </div>
</div></div>
<div class="col">
<div class="border border-dark">
<div class="bg-secondary text-white"><span id="bmsCycle">No Data</span></div>
</div>
</div>
</div>
<div class="row gx-0 mb-2">
<div class="col-4">
<div class="border border-dark">
<div class="bg-primary text-white">Články 1-4: </div>
</div></div>
<div class="col">
<div class="border border-dark">
<div class="bg-secondary text-white"><span id="cell1">No Data</span>&nbsp;<span id="cell2"></span>&nbsp;<span id="cell3"></span>&nbsp;<span id="cell4"></span></div>
</div>
</div>
</div>
<div class="row gx-0 mb-2">
<div class="col-4">
<div class="border border-dark">
<div class="bg-primary text-white">Články 5-8: </div>
</div></div>
<div class="col">
<div class="border border-dark">
<div class="bg-secondary text-white"><span id="cell5">No Data</span>&nbsp;<span id="cell6"></span>&nbsp;<span id="cell7"></span>&nbsp;<span id="cell8"></span></div>
</div>
</div>
</div>
<div class="row gx-0 mb-2">
<div class="col-4">
<div class="border border-dark">
<div class="bg-primary text-white">Teploty: </div>
</div></div>
<div class="col">
<div class="border border-dark">
<div class="bg-secondary text-white"><span id="bmsTemp0">No Data</span>&nbsp;<span id="bmsTemp1"></span>&nbsp;<span id="bmsTemp2"></span></div>
</div>
</div>
</div>
</div>
<script>
        $(document).ready(function(load) {
         function fetch() {
        $.ajax({
            url: "livedata",
            data: {},
            type: "get",
            dataType: "json",
               cache: false,
                success: function (data) {
               document.getElementById("solarV").innerHTML = data.solarV+'V ';
               document.getElementById("solarA").innerHTML = data.solarA+'A  ';
               document.getElementById("solarW").innerHTML = data.solarW+'W';
               document.getElementById("batV").innerHTML = data.batV+'V ';
               document.getElementById("batA").innerHTML = data.batA+'A ';
               document.getElementById("batW").innerHTML = data.batW+'W ';
               document.getElementById("dcoutV").innerHTML = data.dcoutV+'V ';
               document.getElementById("dcoutA").innerHTML = data.dcoutA+'A ';
               document.getElementById("dcoutW").innerHTML = data.dcoutW+'W ';
               document.getElementById("acoutV").innerHTML = data.acoutV+'V';
               document.getElementById("acoutW").innerHTML = data.acoutW+'W ';
               document.getElementById("mpptTemp").innerHTML = data.mpptTemp+'°C ';
               document.getElementById("bmsV").innerHTML = data.bmsV+'V ';
               document.getElementById("bmsA").innerHTML = data.bmsA+'A ';
               document.getElementById("bmsSOC").innerHTML = data.bmsSOC+'%% ';
               document.getElementById("bmsCap").innerHTML = data.bmsCap+'Ah ';
               document.getElementById("bmsCycle").innerHTML = data.bmsCycle;
               document.getElementById("cell1").innerHTML = data.cell1+'V ';
               document.getElementById("cell2").innerHTML = data.cell2+'V ';
               document.getElementById("cell3").innerHTML = data.cell3+'V ';
               document.getElementById("cell4").innerHTML = data.cell4+'V ';
               document.getElementById("cell5").innerHTML = data.cell5+'V ';
               document.getElementById("cell6").innerHTML = data.cell6+'V ';
               document.getElementById("cell7").innerHTML = data.cell7+'V ';
               document.getElementById("cell8").innerHTML = data.cell8+'V ';
               document.getElementById("bmsTemp0").innerHTML = data.bmsTemp0+'°C ';
               document.getElementById("bmsTemp1").innerHTML = data.bmsTemp1+'°C ';
               document.getElementById("bmsTemp2").innerHTML = data.bmsTemp2+'°C ';
               document.getElementById("energyDay").innerHTML = data.energyDay+'kWh ';
               document.getElementById("energyTotal").innerHTML = data.energyTotal+'kWh ';
               document.getElementById("uptime").innerHTML = data.uptime;            

            }
        });
        }
        setInterval(fetch, 5000);
        fetch();
        });
</script>

        <div class="text-right">
            <p></p>
            <a class="btn btn-warning" href="/update" role="button">Aktualizace Firmware</a>
            <p></p>
            <a class="btn btn-primary" href="/reboot" role="button">Restart ESP</a>
            &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
            <span style="text-align:right;">Uptime: </span>
            <span  id="uptime">XX:XX:XX </span>
            
            
        </div>
        
<div>
        &nbsp;
</div>
)rawliteral";

const char HTML_HEAD[] PROGMEM = R"rawliteral(
<!DOCTYPE html> <html>
<head>
<meta http-equiv="content-type" content="text/html;charset=UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<link rel="shortcut icon" href="https://cdn-icons-png.flaticon.com/512/3463/3463440.png" />
<link href="https://cdn.jsdelivr.net/npm/bootstrap@5.1.3/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-1BmE4kWBq78iYhFldvKuhfTAU6auU8tT94WrHftjDbrCEXSU1oBoqyl2QvZ6jIW3" crossorigin="anonymous">
<script src="https://ajax.googleapis.com/ajax/libs/jquery/3.6.0/jquery.min.js"></script>
<script src="https://cdn.jsdelivr.net/npm/bootstrap@5.1.3/dist/js/bootstrap.bundle.min.js" integrity="sha384-ka7Sk0Gln4gmtz2MlQnikT1wXgYsOg+OMhuP+IlRH9sENBO0LRn5q+8nbTov4+1p" crossorigin="anonymous"></script>
<title>ALIS FVE</title>
<link rel="icon" type="image/x-icon" href="https://cdn-icons-png.flaticon.com/512/3463/3463440.png">
<body>
    <noscript>
      <strong>We're sorry but it doesn't work properly without JavaScript enabled. Please enable it to continue.</strong>
    </noscript>
<div class="container-md col-md-4">
)rawliteral";

 
const char HTML_FOOT[] PROGMEM = R"rawliteral(
<figure class="text-center">EPEVER & JBD BMS Reader V0.2</figure>
</div>
</body>
</html>
)rawliteral";