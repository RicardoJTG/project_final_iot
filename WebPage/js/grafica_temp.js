function datos_t(){
    var datos = [],
    tiempo = (new Date()).getTime(), i;
    for( i = -10; i <= 0; i += 1){
        datos.push({
            x: tiempo + i * 1000,
            y: -1
        });
    }
    return datos;
}

var config_t = {
    // The type of chart we want to create
    type: 'line',

    // The data for our dataset
    data: {
        datasets: [{
            label: 'Temperatura Sala',
            backgroundColor: 'rgb(255, 0, 0)',
            borderColor: 'rgb(255, 0, 0)',
            data: datos_t(0),
            fill: false
        }]
    },

    // Configuration options go here
    options: {
        title: {
            display: true,
            text: 'Temperatura en tiempo real'
        },
        scales : {
            xAxes: [{
                type: 'time'
            }],
            yAxes: [{
                display: true,
                ticks: {
                    beginAtzero: true,
                    steps: 10,
                    stepValue: 5,
                    max: 100
                }
            }]
        }
    }
}

var ctx_t = document.getElementById('grafica_temp').getContext('2d');
var grafica_temp = new Chart(ctx_t,config_t);

function update() {        
    var time = (new Date()).getTime();
    config_t.data.datasets.forEach(function(dataset) {
        dataset.data.shift();
        dataset.data.push({x: time ,y: temp_hall});
    });
    grafica_temp.update();
}

setInterval(update, 1000);