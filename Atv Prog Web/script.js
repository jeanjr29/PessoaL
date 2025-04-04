// Simula o banco de dados na memória local
const donosPets = JSON.parse(localStorage.getItem('donosPets')) || [];
let usuarioLogado = null; // Armazena o usuário logado
const senhaPadraoVet = "vet123"; // Senha padrão para veterinários

// Função auxiliar para limpar campos de formulários
function limparCampos(formularioId) {
    const formulario = document.getElementById(formularioId);
    if (formulario) {
        formulario.querySelectorAll('input, textarea').forEach(campo => campo.value = '');
    }
}

// Função para cadastrar donos
function cadastrarDono() {
    const nome = document.getElementById('nome-dono').value;
    const email = document.getElementById('email-dono').value;
    const senha = document.getElementById('senha-dono').value;

    if (nome && email && senha) {
        donosPets.push({ nome, email, senha, consultas: [] });
        localStorage.setItem('donosPets', JSON.stringify(donosPets));
        alert('Cadastro realizado com sucesso!');
        limparCampos('cadastro-form');
    } else {
        alert('Por favor, preencha todos os campos!');
    }
}

// Função para realizar login
function realizarLogin() {
    const email = document.getElementById('email-login').value;
    const senha = document.getElementById('senha-login').value;

    const dono = donosPets.find(dono => dono.email === email && dono.senha === senha);

    if (dono) {
        usuarioLogado = { tipo: 'dono', dados: dono };
        alert(`Bem-vindo, ${dono.nome}!`);
        showScreen('area-paciente'); // Redireciona para a área do paciente
    } else if (email === "veterinario@clinica.com" && senha === senhaPadraoVet) {
        usuarioLogado = { tipo: 'veterinario' };
        alert("Bem-vindo, Veterinário!");
        showScreen('area-veterinario'); // Redireciona para a área do veterinário
    } else {
        alert('Email ou senha incorretos!');
    }
}

// Função para marcar consulta (Área do Paciente)
function agendarConsulta() {
    const dataConsulta = document.getElementById('data-consulta').value;

    if (dataConsulta && usuarioLogado.tipo === 'dono') {
        usuarioLogado.dados.consultas.push({ dataConsulta, diagnostico: null, medicamentos: null });
        localStorage.setItem('donosPets', JSON.stringify(donosPets));
        alert('Consulta marcada com sucesso!');
        limparCampos('consulta-form');
        exibirHistoricoConsultas();
    } else {
        alert('Preencha todos os campos ou faça login como paciente!');
    }
}

// Função para exibir histórico de consultas (Área do Paciente)
function exibirHistoricoConsultas() {
    const historicoDiv = document.getElementById('historico-consultas');
    historicoDiv.innerHTML = '';

    if (usuarioLogado.tipo === 'dono') {
        const consultas = usuarioLogado.dados.consultas;

        consultas.forEach((consulta, index) => {
            const card = document.createElement('div');
            card.className = 'card';

            const cardHeader = document.createElement('div');
            cardHeader.className = 'card-header';
            cardHeader.textContent = `Consulta #${index + 1} - ${consulta.dataConsulta}`;

            const cardBody = document.createElement('div');
            cardBody.className = 'card-body';
            cardBody.innerHTML = `
                <p><strong>Diagnóstico:</strong> ${consulta.diagnostico || 'Não disponível'}</p>
                <p><strong>Medicamentos:</strong> ${consulta.medicamentos || 'Não disponível'}</p>
            `;

            card.appendChild(cardHeader);
            card.appendChild(cardBody);
            historicoDiv.appendChild(card);
        });
    }
}

// Função para inserir diagnóstico e medicamentos (Área do Veterinário)
function inserirDiagnostico() {
    const nomePet = document.getElementById('nome-pet-diagnostico').value;
    const diagnostico = document.getElementById('diagnostico').value;
    const medicamentos = document.getElementById('medicamentos').value;

    // Procura o dono pelo nome do pet informado
    const dono = donosPets.find(dono =>
        dono.consultas.some(c => c.dataConsulta && c.diagnostico === null)
    );

    if (nomePet && diagnostico && medicamentos && usuarioLogado.tipo === 'veterinario') {
        if (dono) {
            const consulta = dono.consultas.find(c => c.dataConsulta && c.diagnostico === null);
            consulta.diagnostico = diagnostico;
            consulta.medicamentos = medicamentos;

            localStorage.setItem('donosPets', JSON.stringify(donosPets));
            alert('Diagnóstico salvo com sucesso!');
            limparCampos('diagnostico-form');
        } else {
            alert('Consulta não encontrada!');
        }
    } else {
        alert('Preencha todos os campos ou faça login como veterinário!');
    }
}

// Função para alternar telas
function showScreen(screenId) {
    const screens = document.querySelectorAll('.container');
    screens.forEach(screen => {
        screen.style.display = screen.id === screenId ? 'block' : 'none';
    });
}

// Inicializa o sistema
function initializePage() {
    showScreen('inicio'); // Começa na tela inicial
}

// Adiciona eventos ao carregar o DOM
document.addEventListener('DOMContentLoaded', initializePage);
