#include <QWindow>
#include <QMessageBox>
#include <iostream>
#include "whatsprog_main.h"
#include "ui_whatsprog_main.h"
#include "mysocket.h"
#include "whatsprog_dados_cliente.h"
#include <thread>
#include <QMessageBox>
#include <cstdio>
#include <cstdlib>

using namespace std;

/// a variavel global que contem todas as msgs de todas as conversas
extern WhatsProgDadosCliente DC;

/// o socket do cliente, a ser utilizado por todas as threads
extern tcp_mysocket sock;

/// Construtor da janela principal da interface
WhatsProgMain::WhatsProgMain(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WhatsProgMain),
    thread(nullptr),
    pixEnviada(),
    pixRecebida(),
    pixEntregue(),
    pixOther(),
    msgStatus(nullptr),
    loginDialog(nullptr),
    novaConversa(nullptr)
{
    ui->setupUi(this);

    //cria a thread do WhatsProg
    thread = new WhatsProgThread(this);

    // Cria caixas de dialogos de login e de nova conversa
    loginDialog = new WhatsProgLogin(this);
    novaConversa = new WhatsProgNovaConversa(this);

    // A lista da esquerda (conversas)
    ui->tableConversas->setColumnCount(2);
    ui->tableConversas->setShowGrid(false);
    ui->tableConversas->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableConversas->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableConversas->setTabKeyNavigation(false);
    ui->tableConversas->horizontalHeader()->setSectionResizeMode(0,QHeaderView::ResizeToContents);
    ui->tableConversas->horizontalHeader()->setSectionResizeMode(1,QHeaderView::Stretch);
    ui->tableConversas->horizontalHeader()->setSectionsClickable(false);
    // O cabecalho
    ui->tableConversas->setStyleSheet("QHeaderView::section { text-align:center }"
                                      "QHeaderView::section { font: bold }"
                                      "QHeaderView::section { background-color:lightgray }");
    ui->tableConversas->setHorizontalHeaderLabels(QStringList() << "Num" <<  "Usuario");

    // A lista da direita (mensagens)
    ui->tableMensagens->setColumnCount(3);
    ui->tableMensagens->setShowGrid(true);
    ui->tableMensagens->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableMensagens->setSelectionMode(QAbstractItemView::NoSelection);
    ui->tableMensagens->setTabKeyNavigation(false);
    ui->tableMensagens->horizontalHeader()->setSectionResizeMode(0,QHeaderView::ResizeToContents);
    ui->tableMensagens->horizontalHeader()->setSectionResizeMode(1,QHeaderView::Stretch);
    ui->tableMensagens->horizontalHeader()->setSectionResizeMode(2,QHeaderView::ResizeToContents);
    ui->tableMensagens->horizontalHeader()->setSectionsClickable(false);
    // O cabecalho
    ui->tableMensagens->setStyleSheet("QHeaderView::section { text-align:center }"
                                      "QHeaderView::section { font: bold }"
                                      "QHeaderView::section { background-color:lightgray }");
    ui->tableMensagens->setHorizontalHeaderLabels(QStringList() << "Id" << "Mensagem" <<  "St");

    // Os icones do status das mensagens
    QString dir = ".\\";
    QString pixfile;

    pixfile = dir+"status0.png";
    if (!pixEnviada.load(pixfile,"PNG"))
    {
        cerr << "Erro na leitura do pixmap " << pixfile.toStdString() << endl;
    }
    pixfile = dir+"status1.png";
    if (!pixRecebida.load(pixfile,"PNG"))
    {
        cerr << "Erro na leitura do pixmap " << pixfile.toStdString() << endl;
    }
    pixfile = dir+"status2.png";
    if (!pixEntregue.load(pixfile,"PNG"))
    {
        cerr << "Erro na leitura do pixmap " << pixfile.toStdString() << endl;
    }
    pixfile = dir+"status_other.png";
    if (!pixOther.load(pixfile,"PNG"))
    {
        cerr << "Erro na leitura do pixmap " << pixfile.toStdString() << endl;
    }

    // O icone da aplicacao
    QPixmap pixIcon;
    pixfile = dir+"whatsprog_icon.png";
    if (!pixIcon.load(pixfile,"PNG"))
    {
        cerr << "Erro na leitura do pixmap " << pixfile.toStdString() << endl;
    }
    else
    {
        setWindowIcon(QIcon(pixIcon));
    }

    // A conexao dos sinais e slots
    // Falta acrescenta outros sinais e slots

    // Os sinais da WhatsProgMain
    connect(this, &WhatsProgMain::signLogin,
            loginDialog, &WhatsProgLogin::slotLogin);
    connect(this, &WhatsProgMain::signShowNovaConversa,
            novaConversa, &WhatsProgNovaConversa::show);

    // Os sinais da WhatsProgLogin
    connect(loginDialog, &WhatsProgLogin::signConectar,
            this, &WhatsProgMain::slotConectar);

    // Os sinais da WhatsProgNovaConversa
    connect(novaConversa, &WhatsProgNovaConversa::signAtualizaConversas,
            this, &WhatsProgMain::slotExibirConversas);
    connect(novaConversa, &WhatsProgNovaConversa::signAtualizaMensagens,
            this, &WhatsProgMain::slotExibirMensagens);

    // Precisa conectar
    // os sinais da classe que contem a thread do cliente
    connect(thread, SIGNAL(confimarLeitura(IterConversa)),
            this, SLOT(slotExibirMensagens(IterConversa)));

    connect(thread, SIGNAL(novaMensagem(IterConversa)),
            this, SLOT(slotExibirConversas()));

    connect(this, SIGNAL(initThread()),
            thread, SLOT(confirmarThread()));

    connect(this, SIGNAL(endThread()),
            thread, SLOT(finalizarThread()));

    connect(thread, SIGNAL(desconectar()),
            this, SLOT(interface_Desconectada()));

    // A barra de status
    msgStatus = new QLabel("?");
    statusBar()->addWidget(msgStatus);

    // Coloca a interface em modo desconectado
    on_actionDesconectar_triggered();
}

WhatsProgMain::~WhatsProgMain()
{
    delete ui;
}

/// Redesenha a janela de conversas
void WhatsProgMain::slotExibirConversas()
{
    // Redesenha toda a tabela de conversas
    // O redesenho eh diferente caso o cliente esteja conectado ou nao.
    //
    // Testa se o socket estah conectado e
    // os dados de conexao estao corretamente definidos
    //
    ui->tableConversas->clearContents();
    ui->tableConversas->setRowCount(DC.size());

    QLabel *prov;
    unsigned cont(0);

    for(iterConversa it=DC.begin(); it!=DC.end(); it++)
    {
        QString nMensagens = QString::number(it->getNumeroMensagensRecebidas()) + "/" + QString::number(it->size());

        prov = new QLabel(nMensagens);
        prov->setAlignment(Qt::AlignCenter);

        ui->tableConversas->setCellWidget(cont, 0, prov);
        QString destinatario = it->getCorrespondente().c_str();

        prov = new QLabel(destinatario);
        prov->setAlignment(Qt::AlignCenter);

        ui->tableConversas->setCellWidget(cont, 1, prov);
        if(it->getNumeroMensagensRecebidas() > 0)
        {
            ui->tableConversas->cellWidget(cont, 0)->setStyleSheet("background-color:lightgreen");
            ui->tableConversas->cellWidget(cont, 1)->setStyleSheet("background-color:lightgreen");
        }
        else
        {
            ui->tableConversas->cellWidget(cont, 0)->setStyleSheet("background-color:white");
            ui->tableConversas->cellWidget(cont, 1)->setStyleSheet("background-color:white");
        }

        cont++;
    }
    ui->tableConversas->viewport()->update();
}

/// Redesenha a janela de mensagens
void WhatsProgMain::slotExibirMensagens()
{
    // Redesenha toda a tabela de mensagens
    // O redesenho eh diferente caso o (cliente esteja conectado e
    // uma conversa esteja selecionada) ou nao.
    //
    // Testa se o socket estah conectado,
    // os dados de conexao estao corretamente definidos e
    // uma conversa estah selecionada
    //
    iterConversa it = DC.getConversaAtual();
    bool enviado = true;
        QLabel *prov;

        ui->tableMensagens->clearContents();
        ui->tableMensagens->setRowCount(it->size());
        for(unsigned i=0; i<unsigned(it->size()); i++)
        {
            int id = it->getMensagem(i).getId();
            QString idString = QString::number(id);
            prov = new QLabel(idString.toStdString().c_str());
            prov->setAlignment(Qt::AlignCenter);
            if(it->getMensagem(i).getRemetente() == DC.getMeuUsuario()) prov->setStyleSheet("background: lightgreen");
            ui->tableMensagens->setCellWidget(i, 0, prov);

            if(it->getMensagem(i).getRemetente() == DC.getMeuUsuario()){
                string mensagem = it->getMensagem(i).getTexto();
                prov = new QLabel(mensagem.c_str());
                prov->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
                prov->setStyleSheet("background: lightgreen");
                ui->tableMensagens->setCellWidget(i, 1, prov);
                if (it->getMensagem(i).getStatus() == MsgStatus::MSG_ENVIADA){
                    prov = new QLabel();
                    prov->setPixmap(pixEnviada);
                }
                if (it->getMensagem(i).getStatus() == MsgStatus::MSG_RECEBIDA){
                    prov = new QLabel();
                    prov->setPixmap(pixRecebida);
                }
                if (it->getMensagem(i).getStatus() == MsgStatus::MSG_ENTREGUE){
                    prov = new QLabel();
                    prov->setPixmap(pixEntregue);
                }
                /*if (it->getMensagem(i).getStatus() == MsgStatus::MSG_LIDA){
                    prov = new QLabel();
                    prov->setPixmap(pixLida);
                }*/
                prov->setScaledContents(true);
                ui->tableMensagens->setCellWidget(i, 2, prov);
            }

            else
            {
                string mensagem = it->getMensagem(i).getTexto();
                prov = new QLabel(mensagem.c_str());
                prov->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
                ui->tableMensagens->setCellWidget(i, 1, prov);

                prov = new QLabel();
                prov->setPixmap(pixOther);
                prov->adjustSize();
                ui->tableMensagens->setCellWidget(i, 2, prov);
                if(it->getMensagem(i).getStatus() == MsgStatus::MSG_ENTREGUE){
                    //if(enviado) enviado = (sock.write_int(CMD_MSG_LIDA) != mysocket_status::SOCK_ERROR);
                    if(enviado) enviado = (sock.write_int(it->getMensagem(i).getId()) != mysocket_status::SOCK_ERROR);
                    if(enviado) enviado = (sock.write_string(it->getMensagem(i).getRemetente()) != mysocket_status::SOCK_ERROR);
                    /*if(enviado){
                        DC.setStatus(it,i,MsgStatus::MSG_LIDA);
                    }*/
                }
            }
            ui->tableMensagens->viewport()->update();
        }

        ui->tableConversas->viewport()->update();

        if (!enviado){
            sock.close();
            QMessageBox::critical(this, "ERRO", "Falha no envio das confimacoes de visualizacao");
        }
}

/// Redesenha a barra de status
void WhatsProgMain::atualizaEstadoConexao()
{
    // Atualiza a msg na barra de status
    // A atualizacao o eh diferente caso o cliente esteja conectado ou nao.
    //
    // Testa se o socket estah conectado e
    // os dados de conexao estao corretamente definidos
    //
    // Falta fazer
}

/// Conecta-se ao servidor
void WhatsProgMain::slotConectar(const QString &IP, const QString &login,
                                 const QString &senha, bool novoUsuario )
{
    // Testa todos os parametros
    // Depois faz a conexao com servidor
    //
    string newIp = IP.toStdString();
    string newUsuario = login.toStdString();
    string newSenha = senha.toStdString();
   /* if(novoUsuario)
        QMessageBox::critical(this, "Erro ao criar usuário", "Houve algum problema");
    else
        QMessageBox::critical(this, "Erro ao logar usuário", "Houve algum problema");
    */
    if (sock.connected())
    {
        QMessageBox::critical(this, "ERRO", "Esta funcao soh deve ser chamada quando o cliente estah desconectado");
        //return false;
    }
    bool conexaoOK(true);
    int32_t cmd;
    if (sock.connect(IP.toStdString(), PORTA_WHATSPROG) != mysocket_status::SOCK_OK)
    {
        sock.close();
        //return false;
    }
    if (novoUsuario)
    {
        if (conexaoOK) conexaoOK = (sock.write_int(CMD_NEW_USER) != mysocket_status::SOCK_ERROR);
    }
    else
    {
        if (conexaoOK) conexaoOK = (sock.write_int(CMD_LOGIN_USER) != mysocket_status::SOCK_ERROR);
        //listar_Conversas();
    }
    //if (!conexaoOK) return false;
    if (conexaoOK) conexaoOK = (sock.write_string(login.toStdString()) != mysocket_status::SOCK_ERROR);
    if (conexaoOK) conexaoOK = (sock.write_string(senha.toStdString()) != mysocket_status::SOCK_ERROR);
    if (!conexaoOK)
    {
        sock.close();
        //return false;
    }
    conexaoOK = (sock.read_int(cmd,1000*TIMEOUT_WHATSPROG) == mysocket_status::SOCK_OK);
    if (conexaoOK) conexaoOK = (cmd == CMD_LOGIN_OK);
    if (!conexaoOK)
    {
        sock.close();
        //return false;
    }
    DC.setServidorUsuario(IP.toStdString(),login.toStdString());
    DC.ler();
    if (DC.size() == 1)
    {
        DC.setConversaAtual(DC.begin());
    }
    string user_servidor = ("CONECTADO: " + login + "@" + IP).toStdString();
    msgStatus->setText(user_servidor.c_str());
    statusBar()->insertWidget(0, msgStatus);
    ui->menuConversa->setEnabled(1);

    emit initThread();
}

/// Exibe um pop-up com mensagem de erro
void WhatsProgMain::slotExibirErroMensagem(string S)
{
    QMessageBox::warning(this, "WhatsProg - Erro", QString::fromStdString(S));
}

void WhatsProgMain::on_actionNovo_usuario_triggered()
{
    // Exibe a janela de login para novo usuario (true)
    emit signLogin(true);
}

void WhatsProgMain::on_actionUsuario_existente_triggered()
{
    // Exibe a janela de login para usuario existente (false)
    emit signLogin(false);
}

/// Coloca a interface em modo desconectado
/// Desconecta o socket, limpa o servidor e usuario atuais,
/// limpa todas as conversas, redesenha todas as janelas
void WhatsProgMain::on_actionDesconectar_triggered()
{
    ui->tableConversas->clearContents();
    ui->tableMensagens->clearContents();

    msgStatus->setText("NAO CONECTADO");
    statusBar()->insertWidget(0, msgStatus);

    ui->lineEditMensagem->setEnabled(0);
    ui->menuMensagens->setEnabled(0);
    ui->menuConversa->setEnabled(0);
    ui->tableMensagens->setRowCount(0);

    emit endThread();

    // Redesenha todas as janelas
    slotExibirConversas();
    slotExibirMensagens();
    atualizaEstadoConexao();
}

void WhatsProgMain::on_actionSair_triggered()
{
    QCoreApplication::quit();
}

void WhatsProgMain::on_actionNova_conversa_triggered()
{
    emit signShowNovaConversa();
}

void WhatsProgMain::on_actionRemover_conversa_triggered()
{
    // Falta fazer

    // Sinaliza que houve alteracao na janela de Conversas
    slotExibirConversas();
    // Sinaliza que houve alteracao na janela de Mensagens
    slotExibirMensagens();
}

void WhatsProgMain::on_actionApagar_mensagens_triggered()
{
    // Falta fazer

    // Sinaliza que houve alteracao no numero de msgs de uma conversa
    slotExibirConversas();
    // Sinaliza que houve alteracao na janela de Mensagens
    slotExibirMensagens();
}

void WhatsProgMain::on_tableConversas_activated(const QModelIndex &index)
{
    on_tableConversas_clicked(index);
}

void WhatsProgMain::on_tableConversas_clicked(const QModelIndex &index)
{
    // Falta fazer

    // Sinaliza que houve alteracao na conversa selecionada
    slotExibirConversas();
    // Sinaliza que houve alteracao na janela de Mensagens
    slotExibirMensagens();
}

void WhatsProgMain::on_lineEditMensagem_returnPressed()
{
    // Falta fazer

    // Sinaliza que houve alteracao no numero de msgs de uma conversa
    slotExibirConversas();
    // Sinaliza que houve alteracao na janela de Mensagens
    slotExibirMensagens();
}
