
var gpio_0_status = false;
var gpio_1_status = false;

var connection = new WebSocket('ws://' + location.hostname + ':81/', ['arduino']);
connection.onopen = function () {
  connection.send('Connect ' + new Date());
};

connection.onerror = function (error) {
  console.log('WebSocket Error ', error);
};
connection.onmessage = function (e) {
  console.log('Server: ', e.data);
};
connection.onclose = function () {
  console.log('WebSocket connection closed');
};

function IO_handler () {

  var gpio0status = document.getElementById('gpio0status');
  var gpio1status = document.getElementById('gpio1status');

  var message = [('#' + gpio0status.toString(16)),('#' + gpio1status.toString(16))];
  
  console.log('GPIO status: ' + message);
  connection.send(message);

}

// function rainbowEffect () {
//   rainbowEnable = ! rainbowEnable;
//   if (rainbowEnable) {
//     connection.send("R");
//     document.getElementById('rainbow').style.backgroundColor = '#00878F';
//     document.getElementById('r').className = 'disabled';
//     document.getElementById('g').className = 'disabled';
//     document.getElementById('b').className = 'disabled';
//     document.getElementById('r').disabled = true;
//     document.getElementById('g').disabled = true;
//     document.getElementById('b').disabled = true;
//   } else {
//     connection.send("N");
//     document.getElementById('rainbow').style.backgroundColor = '#999';
//     document.getElementById('r').className = 'enabled';
//     document.getElementById('g').className = 'enabled';
//     document.getElementById('b').className = 'enabled';
//     document.getElementById('r').disabled = false;
//     document.getElementById('g').disabled = false;
//     document.getElementById('b').disabled = false;
//     GPIO_handler();
//   }
// }