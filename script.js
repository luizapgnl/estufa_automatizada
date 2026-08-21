// ======================================================
// CONFIGURAÇÃO DO ESP32
// ======================================================

// Coloque aqui o IP mostrado no Monitor Serial da Arduino IDE
const ESP32_IP = "http://192.168.1.15";

// Intervalo de atualização
const INTERVALO = 2000;


// ======================================================
// ELEMENTOS DA INTERFACE
// ======================================================

const statusText = document.getElementById("statusText");
const pingDot = document.getElementById("pingDot");
const lastUpdate = document.getElementById("lastUpdate");

const tempValue = document.getElementById("tempValue");
const humidityValue = document.getElementById("humidityValue");
const luxValue = document.getElementById("luxValue");
const soilValue = document.getElementById("soilValue");
const soilBar = document.getElementById("soilBar");

const pumpToggle = document.getElementById("pumpToggle");
const fanToggle = document.getElementById("fanToggle");
const lightToggle = document.getElementById("lightToggle");

const pumpStatus = document.getElementById("pumpStatus");
const fanStatus = document.getElementById("fanStatus");
const lightStatus = document.getElementById("lightStatus");

const pumpIcon = document.getElementById("pumpIcon");
const fanIcon = document.getElementById("fanIcon");
const lightIcon = document.getElementById("lightIcon");


// ======================================================
// MODO ESCURO
// ======================================================

document.getElementById("themeToggle").addEventListener("click", () => {
    document.documentElement.classList.toggle("dark");
});


// ======================================================
// GRÁFICO
// ======================================================

const ctx = document
    .getElementById("tempHumidityChart")
    .getContext("2d");

const chart = new Chart(ctx, {

    type: "line",

    data: {

        labels: [],

        datasets: [

            {
                label: "Temperatura (°C)",
                data: [],
                borderColor: "#f97316",
                backgroundColor: "rgba(249, 115, 22, 0.1)",
                tension: 0.4,
                fill: true
            },

            {
                label: "Umidade (%)",
                data: [],
                borderColor: "#3b82f6",
                backgroundColor: "rgba(59, 130, 246, 0.1)",
                tension: 0.4,
                fill: true
            }

        ]

    },

    options: {

        responsive: true,
        maintainAspectRatio: false,

        scales: {

            y: {
                beginAtZero: false
            }

        },

        plugins: {

            legend: {
                display: true
            }

        }

    }

});


// ======================================================
// FUNÇÃO PARA ATUALIZAR O GRÁFICO
// ======================================================

function atualizarGrafico(temperatura, umidade) {

    const agora = new Date();

    const horario = agora.toLocaleTimeString("pt-BR", {
        hour: "2-digit",
        minute: "2-digit",
        second: "2-digit"
    });

    chart.data.labels.push(horario);

    chart.data.datasets[0].data.push(temperatura);
    chart.data.datasets[1].data.push(umidade);


    // Mantém somente os últimos 10 valores
    if (chart.data.labels.length > 10) {

        chart.data.labels.shift();

        chart.data.datasets[0].data.shift();
        chart.data.datasets[1].data.shift();

    }

    chart.update("none");
}


// ======================================================
// ATUALIZAÇÃO DOS ESTADOS DOS ATUADORES
// ======================================================

function atualizarBomba(estado) {

    pumpToggle.checked = estado;

    pumpStatus.innerText = estado
        ? "Ativado"
        : "Desativado";

    if (estado) {

        pumpIcon.className =
            "p-2.5 rounded-lg bg-blue-500 text-white";

    } else {

        pumpIcon.className =
            "p-2.5 rounded-lg bg-slate-100 dark:bg-slate-800 text-slate-400";

    }
}


function atualizarCooler(estado) {

    fanToggle.checked = estado;

    fanStatus.innerText = estado
        ? "Ativado"
        : "Desativado";

    if (estado) {

        fanIcon.className =
            "p-2.5 rounded-lg bg-orange-500 text-white animate-spin";

    } else {

        fanIcon.className =
            "p-2.5 rounded-lg bg-slate-100 dark:bg-slate-800 text-slate-400";

    }
}


function atualizarLuz(estado) {

    lightToggle.checked = estado;

    lightStatus.innerText = estado
        ? "Ativado"
        : "Desativado";

    if (estado) {

        lightIcon.className =
            "p-2.5 rounded-lg bg-amber-500 text-white";

    } else {

        lightIcon.className =
            "p-2.5 rounded-lg bg-slate-100 dark:bg-slate-800 text-slate-400";

    }
}


// ======================================================
// ATUALIZAÇÃO DA INTERFACE
// ======================================================

function atualizarInterface(data) {

    const temperatura = Number(data.temp);
    const umidade = Number(data.umidade);
    const luminosidade = Number(data.luz);
    const solo = Number(data.soloPct);


    // ------------------------------------------
    // TEMPERATURA
    // ------------------------------------------

    tempValue.innerHTML =
        `${temperatura.toFixed(1)}
        <span class="text-lg font-normal text-slate-500">°C</span>`;


    // ------------------------------------------
    // UMIDADE DO AR
    // ------------------------------------------

    humidityValue.innerHTML =
        `${umidade.toFixed(0)}
        <span class="text-lg font-normal text-slate-500">%</span>`;


    // ------------------------------------------
    // LUMINOSIDADE
    // ------------------------------------------

    luxValue.innerHTML =
        `${luminosidade}
        <span class="text-lg font-normal text-slate-500">%</span>`;


    // ------------------------------------------
    // SOLO
    // ------------------------------------------

    soilValue.innerHTML =
        `${solo}
        <span class="text-lg font-normal text-slate-500">%</span>`;

    soilBar.style.width = `${solo}%`;


    // ------------------------------------------
    // ATUADORES
    // ------------------------------------------

    atualizarBomba(data.bombaState);

    atualizarCooler(data.coolerState);

    atualizarLuz(data.luzState);


    // ------------------------------------------
    // HORÁRIO DA ATUALIZAÇÃO
    // ------------------------------------------

    const agora = new Date();

    lastUpdate.innerText =
        agora.toLocaleTimeString("pt-BR");


    // ------------------------------------------
    // GRÁFICO
    // ------------------------------------------

    atualizarGrafico(
        temperatura,
        umidade
    );


    // ------------------------------------------
    // ALERTAS VISUAIS
    // ------------------------------------------

    verificarAlertas(
        temperatura,
        umidade,
        solo
    );
}


// ======================================================
// ALERTAS
// ======================================================

function verificarAlertas(temperatura, umidade, solo) {

    // Temperatura crítica
    if (temperatura > 30) {

        tempValue.classList.add("text-red-500");

    } else {

        tempValue.classList.remove("text-red-500");

    }


    // Umidade muito alta
    if (umidade > 85) {

        humidityValue.classList.add("text-red-500");

    } else {

        humidityValue.classList.remove("text-red-500");

    }


    // Solo crítico
    if (solo < 30) {

        soilValue.classList.add("text-red-500");

        soilBar.classList.remove("bg-cyan-500");
        soilBar.classList.add("bg-red-500");

    } else {

        soilValue.classList.remove("text-red-500");

        soilBar.classList.remove("bg-red-500");
        soilBar.classList.add("bg-cyan-500");

    }
}


// ======================================================
// BUSCAR DADOS DO ESP32
// ======================================================

async function buscarDados() {

    try {

        const resposta = await fetch(
            `${ESP32_IP}/data`,
            {
                method: "GET",
                cache: "no-cache"
            }
        );


        if (!resposta.ok) {

            throw new Error(
                `HTTP ${resposta.status}`
            );

        }


        const data = await resposta.json();


        console.log("Dados recebidos:", data);


        // ESP32 conectado
        statusText.innerText = "ESP32 Online";

        pingDot.className =
            "w-2 h-2 rounded-full bg-emerald-500 animate-ping";


        // Atualiza dashboard
        atualizarInterface(data);


    } catch (erro) {

        console.error(
            "Erro ao conectar ao ESP32:",
            erro
        );


        // ESP32 desconectado
        statusText.innerText =
            "ESP32 Desconectado";


        pingDot.className =
            "w-2 h-2 rounded-full bg-red-500";


        lastUpdate.innerText =
            "Sem conexão";

    }

}


// ======================================================
// ENVIAR COMANDO PARA O ESP32
// ======================================================

async function toggleActuator(tipo, estado) {

    try {

        const valor = estado ? 1 : 0;


        const url =
            `${ESP32_IP}/toggle?type=${tipo}&state=${valor}`;


        const resposta = await fetch(url);


        if (!resposta.ok) {

            throw new Error(
                "Erro ao enviar comando"
            );

        }


        console.log(
            `Atuador ${tipo}:`,
            estado ? "LIGADO" : "DESLIGADO"
        );


        // Atualiza imediatamente a interface
        if (tipo === "bomba") {

            atualizarBomba(estado);

        }

        if (tipo === "cooler") {

            atualizarCooler(estado);

        }

        if (tipo === "luz") {

            atualizarLuz(estado);

        }


    } catch (erro) {

        console.error(
            "Erro ao controlar atuador:",
            erro
        );

        alert(
            "Não foi possível enviar o comando para o ESP32."
        );

    }

}


// ======================================================
// INICIALIZAÇÃO
// ======================================================

console.log(
    "Dashboard da Estufa iniciado."
);

console.log(
    "ESP32:",
    ESP32_IP
);


// Primeira leitura
buscarDados();


// Atualiza a cada 2 segundos
setInterval(
    buscarDados,
    INTERVALO
);
