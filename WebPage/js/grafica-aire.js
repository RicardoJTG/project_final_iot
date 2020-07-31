function datos(){
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

var config = {
    // The type of chart we want to create
    type: 'line',

    // The data for our dataset
    data: {
        datasets: [{
            label: 'Calidad del aire',
            backgroundColor: 'rgb(27, 255, 0)',
            borderColor: 'rgb(27, 255, 0)',
            data: datos(0),
            fill: false
        }]
    },

    // Configuration options go here
    options: {
        title: {
            display: true,
            text: 'Datos en tiempo real'
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
                    max: 3000
                }
            }]
        }
    }
}

var ctx = document.getElementById('grafica_ca').getContext('2d');
var grafica_ca = new Chart(ctx,config);

function update() {        
    var time = (new Date()).getTime();
    config.data.datasets.forEach(function(dataset) {
        dataset.data.shift();
        dataset.data.push({x: time ,y: calidad_air});
    });
    grafica_ca.update();
}

setInterval(update, 1000);