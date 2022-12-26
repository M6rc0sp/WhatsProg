#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include "whatsprog_dados_servidorFAKE.h"

using namespace std;

/// O flag que indica que o software deve encerrar todas as threads
extern bool fim;

/// Funcao auxiliar que imprime um comando tratado pelo servidor
void imprimeComando(bool recebido, const string& user, ComandoWhatsProg cmd,
                    int id, const string& user2)
{
    cout << "CMD ";
    if (recebido) cout << "REC DE ";
    else cout << "ENV P/ ";
    cout << user << ": " << nome_cmd(cmd);
    if (id > 0) cout << ',' << id;
    if (user2.size() > 0) cout << ',' << user2;
    cout << endl;
}

/// Funcao auxiliar que imprime um comando enviado pelo servidor
void imprimeComandoEnviado(const string& user, ComandoWhatsProg cmd,
                           int id=-1, const string& user2="")
{
    imprimeComando(false, user, cmd, id, user2);
}

/// Funcao auxiliar que imprime um comando recebido pelo servidor
void imprimeComandoRecebido(const string& user, ComandoWhatsProg cmd,
                            int id=-1, const string& user2="")
{
    imprimeComando(true, user, cmd, id, user2);
}

/// Funcoes auxiliares para impressao
ostream& operator<<(ostream& O, const Mensagem& M)
{
    O << M.getId() << ';' << nome_status(M.getStatus()) << ';'
      << M.getRemetente() << ';' << M.getDestinatario() << ';'
      << M.getTexto();
    return O;
}

ostream& operator<<(ostream& O, const Usuario& U)
{
    O << U.getLogin() << ';' << U.getLastId();
    return O;
}

/// CLASSE USUARIO

/// Construtor default
Usuario::Usuario(): login(""), senha(""), s(), last_id(0) {}

/// Funcao de consulta ao valor para login
const string& Usuario::getLogin() const
{
    return login;
}

/// Fixa login e senha do usuario
/// Retorna true se OK; false em caso de erro
bool Usuario::setUsuario(const string& L, const string& S)
{
    if (!testarNomeUsuario(L) || !testarSenha(S))
    {
        return false;
    }
    login = L;
    senha = S;
    return true;
}

/// Valida uma senha, comparando com a senha armazenada
bool Usuario::validarSenha(const string& S) const
{
    return (S == senha);
}

/// Funcao de acesso ao socket do usuario
const tcp_mysocket& Usuario::getSocket() const
{
    return s;
}

/// Alteracao do socket de um usuario
void Usuario::swapSocket(tcp_mysocket& S)
{
    s.swap(S);
}

/// Consulta do estado do socket
bool Usuario::connected() const
{
    return s.connected();
}

bool Usuario::closed() const
{
    return s.closed();
}

/// Funcoes de envio de dados via socket
mysocket_status Usuario::read_int(int32_t& num, long milisec) const
{
    return s.read_int(num,milisec);
}

mysocket_status Usuario::write_int(int32_t num) const
{
    return s.write_int(num);
}

mysocket_status Usuario::read_string(string& msg, long milisec) const
{
    return s.read_string(msg,milisec);
}

mysocket_status Usuario::write_string(const string& msg) const
{
    return s.write_string(msg);
}

/// Fechamento do socket
void Usuario::close()
{
    s.close();
}

/// Consulta da ultima ID do usuario
int32_t Usuario::getLastId() const
{
    return last_id;
}

/// Alteracao da ultima ID do usuario
bool Usuario::setLastId(int32_t ID)
{
    if (ID<=0 || ID<=last_id)
    {
        return false;
    }
    last_id = ID;
    return true;
}

/// Reinicializa a ID do usuario
void Usuario::resetId()
{
    last_id=0;
}

/// Teste de igualdade com uma string (testa se a string eh igual ao login)
bool Usuario::operator==(const string& L) const
{
    return login==L;
}

/// CLASSE WHATSPROGDADOSSERVIDOR

/// Funcoes de acesso aas funcionalidades basicas dos sockets
mysocket_status WhatsProgDadosServidor::listen(const char *port, int nconex)
{
    return c.listen(port,nconex);
}

/// Fecha os sockets de conexao e de todos os usuarios conectados
void WhatsProgDadosServidor::closeSockets()
{
    c.close();
    itUser iUser;
    // No servidor FAKE, soh existe um usuario
    if (iUser->connected()) iUser->close();
}

/*globais
std::list<Usuario> listUsuario;
listUsuario::iterator itUser;

std::list<Mensagem> listMensagem;
listMensagem::iterator itMsg; //aqui já se faz o buffer
//fim global*/

/* **************************************************************************************
ATENCAO: a parte a seguir da implementacao do servidor FAKE ***NAO*** pode ser adaptada
para o servidor real, a nao ser que seja feita uma analise cuidadosa.
************************************************************************************** */
/// Envia uma mensagem "i" que esteja no buffer com status MSG_RECEBIDA
/// e cujo destinatario seja o usuario conectado (caso de uso S.3)
/// Apos o envio, altera o status da msg enviada para MSG_ENTREGUE
/// Em seguida, simula o envio da confirmacao de entrega e remove do buffer
/// No servidor real deveria ser:
/// void WhatsProgDadosServidor::enviarRecebidas(itMsg iMsg, itUser iDest)
void WhatsProgDadosServidor::enviarMensagem(itMsg& iMsg, itUser& iDest)
{
    //Mensagem M; // No real, haveria um iterator para msg, nao precisaria criar variavel
    try
    {
        // Testa os parametros
        //bool findMsg = find(listMsg.begin(), listMsg.end(), *iMsg) != listMsg.end();
        //testar de user é o end() ou a msg ou se o user está desconectado
        testDestStatus test_dest(iDest->getLogin(), MsgStatus::MSG_RECEBIDA);

        if (!iDest->connected()) throw 1;
        //while (iDest->connected() && iMsg != msgs.end())
        //{
            //itBuffer = find(itBuffer, buffer.end(), test_dest);
            if (iMsg != listMsg.end() && iDest != listUser.end())
            {
                //M = paraUsuario[i];
                if (iMsg->getStatus() !=  MsgStatus::MSG_RECEBIDA ||
                        iMsg->getDestinatario() != iDest->getLogin()) throw 1;

                // Envia a mensagem pelo socket
                if (iDest->write_int(CMD_NOVA_MSG) != mysocket_status::SOCK_OK) throw 2;
                if (iDest->write_int(iMsg->getId()) != mysocket_status::SOCK_OK) throw 3;
                if (iDest->write_string(iMsg->getRemetente()) != mysocket_status::SOCK_OK) throw 4;
                if (iDest->write_string(iMsg->getTexto()) != mysocket_status::SOCK_OK) throw 5;

                // Mensagem enviada
                imprimeComandoEnviado(iDest->getLogin(), CMD_NOVA_MSG,
                                      iMsg->getId(), iMsg->getRemetente());
                iMsg->setStatus(MsgStatus::MSG_ENTREGUE);
                listMsg.push_back(*iMsg);

                // Procura o usuario remetente
                /* No servidor FAKE soh compara com os 2 nomes simulados */
                if (iMsg->getRemetente()!="userfake1" && iMsg->getRemetente()!="userfake2") throw 6;

                // Remetente existe. Testa se estah conectado
                // Se sim, envia a confirmacao de entrega da mensagem
                // Usa a funcao auxiliar enviarConfirmacao
                /* No servidor FAKE nao faz teste jah que usuarios simulados estao sempre "conectados"
                   nem envia a confirmacao para o remetente simulado
                   Nao usa a funcao auxiliar e jah faz aqui mesmo a impressao do comando
                   e a eliminacao da msg do buffer */
                {
                    // Mensagem enviada
                    imprimeComandoEnviado(iMsg->getRemetente(), CMD_MSG_ENTREGUE, iMsg->getId(), "");
                    // Remove a msg do buffer
                    listMsg.pop_back();
                }
            }
        //}
    }
    catch (int erro)
    {
        if (erro>=2 && erro<=5)
        {
            // Desconecta o destinatario se houve erro no envio pelo socket
            iDest->close();
        }
        cerr << "Erro " << erro << " no envio da mensagem para destinatario "
             << iDest->getLogin() << endl;
    }
}

/* **************************************************************************************
ATENCAO: a parte a seguir da implementacao do servidor FAKE ***NAO*** pode ser adaptada
para o servidor real, a nao ser que seja feita uma analise cuidadosa.
************************************************************************************** */
/// Envia uma confirmacao de entrega de mensagem "i"
/// que esteja no buffer com status MSG_ENTREGUE
/// e cujo remetente seja o usuario conectado (caso de uso S.4)
/// Apos o envio da confirmacao, remove a msg do buffer
/// No servidor real deveria ser:
/// void WhatsProgDadosServidor::enviarConfirmacao(itMsg iMsg, itUser iRemet)
void WhatsProgDadosServidor::enviarConfirmacao(itMsg &iMsg, itUser &iRemet)
{
    //Mensagem M; // No real, haveria um iterator para msg, nao precisaria criar variavel
    try
    {
        // Testa os parametros
        //find(listMsg.begin(), listMsg.end(), *iMsg) != listMsg.end();

        //testar de user é o end() ou a msg ou se o user está desconectado
        if (iRemet == listUser.end() || iMsg == listMsg.end() || !iRemet->connected()) throw 1;
        //M = doUsuario[i];
        if (iMsg->getStatus() !=  MsgStatus::MSG_ENTREGUE ||
                iMsg->getRemetente() != iRemet->getLogin()) throw 1;

        // Envia a confirmacao pelo socket
        if (iRemet->write_int(CMD_MSG_ENTREGUE) != mysocket_status::SOCK_OK) throw 2;
        if (iRemet->write_int(iMsg->getId()) != mysocket_status::SOCK_OK) throw 3;

        // Confirmacao enviada
        imprimeComandoEnviado(iRemet->getLogin(), CMD_MSG_ENTREGUE,
                              iMsg->getId(), "");
        // Remove a msg do buffer
        //doUsuario[i] = Mensagem();
        listMsg.pop_back();
    }
    catch (int erro)
    {
        if (erro>=2 && erro<=3)
        {
            // Desconecta o remetente se houve erro no envio pelo socket
            iRemet->close();
        }
        cerr << "Erro " << erro << " no envio de confirmacao de entrega para remetente "
             << iRemet->getLogin() << endl;
    }
}

/* **************************************************************************************
ATENCAO: a parte a seguir da implementacao do servidor FAKE pode ser parcialmente adaptada
para o servidor real, mas requer uma analise muito cuidadosa.
************************************************************************************** */
/// Funcao que efetivamente desempenha as tarefas do servidor
int WhatsProgDadosServidor::main_thread()
{
    // A fila para select (esperar dados em varios sockets)
    mysocket_queue f;

    // O status de retorno das funcoes do socket
    mysocket_status iResult;
    // O comando recebido/enviado
    int32_t cmd;
    // Eventuais parametros de comandos lidos do socket
    int32_t id;
    string destinatario;
    string texto;
    // Mensagem recebida/enviada
    Mensagem M;

    // As ultimas ids dos usuarios fake (soh faz sentido no servidor fake)
    // No servidor real, as last_id estao armazenados em cada usuario na
    // lista de usuarios
    //int32_t last_id[2] = {0,0};
    // O indice do usuario fake
    //int id_fake;

    while (!fim)
    {
        try // Erros graves: catch encerra o servidor
        {
            // Se socket de conexoes nao estah aceitando conexoes, encerra o servidor
            if (!c.accepting()) throw 1; // Erro grave: encerra o servidor

            // Inclui na fila de sockets para o select todos os sockets que eu
            // quero monitorar para ver se houve chegada de dados
            f.clear();
            f.include(c);
            // Soh tem um usuario neste servidor fake...
            // No servidor real, teria que percorrer a lista de usuarios e incluir
            // todos os que estiverem conectados
            for (iUser=listUser.begin(); iUser!=listUser.end(); iUser++)
            {
                if (iUser->getSocket().connected())
                {
                    f.include(iUser->getSocket());
                }
            }

            // Espera que chegue algum dado em qualquer dos sockets da fila
            iResult = f.wait_read(TIMEOUT_WHATSPROG*1000);

            switch (iResult) // resultado do wait_read
            {
            case mysocket_status::SOCK_ERROR: // resultado do wait_read
            default:
                // Erro no select, encerra o servidor
                throw 2; // Erro grave: encerra o servidor
                break;
            case mysocket_status::SOCK_TIMEOUT: // resultado do wait_read
                // Saiu por timeout: nao houve atividade em nenhum socket da fila
                // No servidor real, aproveita para salvar o arquivo de dados
                /* **************************************************************************************
                ATENCAO: ***NADA*** da parte a seguir da implementacao do servidor FAKE pode ser adaptada
                para o servidor real.
                **************************************************************************************
                // NESTE SERVIDOR FAKE, VAMOS UTILIZAR O TIMEOUT PARA SIMULAR O COMPORTAMENTO DOS
                // USUARIOS FAKE: userfake1 e userfake2
                //
                // Simula o eventual envio de msgs dos fakes para o usuario
                // Esta parte simula muito simplificadamente o que deveria acontecer no servidor
                // real apos receber um comando CMD_NEW_MSG de algum outro usuario
                //
                // Tem um usuario definido (mesmo que nao esteja conectado)?
                // Senao, nao envia mensagens simuladas
                if (testarNomeUsuario(iUser->getLogin()))
                {
                  string remetente = "userfake?";
                  for (int i=0; i<2; ++i)
                  {
                    // Soh escreve uma mensagem nova a cada 4 iteracoes em media
                    // e se nao houver mensagem esperando. Caso haja, nao sobreescreve
                    if (rand()%4==0 && paraUsuario[i].getStatus()==MsgStatus::MSG_INVALIDA)
                    {
                      remetente[8] = char('1'+i);  // userfake1 ou userfake2
                      //++last_id[i]; // Incrementa a ultima id antes de enviar proxima msg
                      iUser->setLastID(iUser->getLastID()++);

                      // Forma a mensagem
                      iMsg->setStatus(MsgStatus::MSG_RECEBIDA);
                      iMsg->setRemetente(remetente);
                      iMsg->setDestinatario(iUser->getLogin());
                      iMsg->setId(iUser->getLastID());
                      iMsg->setTexto(string("Msg")+to_string(iUser->getLastID());

                      // Informa que foi "recebida" uma nova mensagem
                      imprimeComandoRecebido(remetente, CMD_NOVA_MSG, iMsg->getId(), iUser->getLogin());
                      // Insere a mensagem no "buffer"
                      //paraUsuario[i]=M;
                      // Informa que foi "enviada" a confirmacao de recebimento para o remetente fake
                      imprimeComandoEnviado(remetente, CMD_MSG_RECEBIDA, iUser->getLastID());

                      // Testa se o destinatario estah conectado
                      // Se sim, envia a mensagem e muda status para MSG_ENTREGUE
                      if (iUser->connected())
                      {
                        enviarMensagem(iMsg, iUser);
                      }
                    } // fim if (rand()%10==0 && ...
                  } // fim for
                } // if (testarNomeUsuario

                // Fim da parte que ***NAO*** pode ser adaptada para o servidor real */
                break; // fim do case mysocket_status::SOCK_TIMEOUT
            case mysocket_status::SOCK_OK: // resultado do wait_read
                // Houve atividade em algum socket da fila
                // Testa em qual socket houve atividade.

                try // Erros nos clientes: catch fecha a conexao com esse cliente
                {
                    // Primeiro, testa os sockets dos clientes
                    // Soh tem um usuario neste servidor fake...
                    // No servidor real, teria que percorrer a lista de usuarios e testar
                    // cada um dos sockets de usuario
                    if (!fim && iUser->connected() && f.had_activity(iUser->getSocket()))
                    {
                        // Leh o comando recebido do cliente
                        iResult = iUser->read_int(cmd);
                        // Pode ser mysocket_status::SOCK_OK, mysocket_status::SOCK_TIMEOUT,
                        // mysocket_status::SOCK_DISCONNECTED ou mysocket_status::SOCK_ERRO
                        // Nao deve ser mysocket_status::SOCK_TIMEOUT porque a funcao read_int
                        // nao foi chamada com tempo maximo
                        if (iResult != mysocket_status::SOCK_OK) throw 1;

                        // Executa o comando lido
                        switch(cmd)
                        {
                        case CMD_NEW_USER:
                            1001;
                            break;
                        case CMD_LOGIN_USER:
                            1002;
                            break;
                        case CMD_LOGIN_OK:
                            1003;
                            break;
                        case CMD_LOGIN_INVALIDO:
                            1004;
                        case CMD_MSG_INVALIDA:
                            1006;
                            break;
                        case CMD_MSG_RECEBIDA:
                            1007;
                            break;
                        case CMD_MSG_ENTREGUE:
                            1008;
                            break;
                        default:
                            // Comando invalido
                            throw 2;
                            break;
                        case CMD_NOVA_MSG:
                            // Receber a msg
                            iResult = iUser->read_int(id, TIMEOUT_WHATSPROG*1000);
                            if (iResult != mysocket_status::SOCK_OK) throw 3;
                            iResult = iUser->read_string(destinatario, TIMEOUT_WHATSPROG*1000);
                            if (iResult != mysocket_status::SOCK_OK) throw 4;
                            iResult = iUser->read_string(texto, TIMEOUT_WHATSPROG*1000);
                            if (iResult != mysocket_status::SOCK_OK) throw 5;

                            // Pedido de nova mensagem
                            imprimeComandoRecebido(iUser->getLogin(), CMD_NOVA_MSG, id, destinatario);

                            // Preenche o status e remetente da mensagem recebida
                            if (!M.setStatus(MsgStatus::MSG_RECEBIDA) ||
                                    !M.setRemetente(iUser->getLogin())) throw 6;

                            // Testa se a id da msg estah correta
                            if (!M.setId(id) || id <= iUser->getLastId()) throw 7;

                            // Testa se o texto da msg estah correto
                            if (!M.setTexto(texto)) throw 8;

                            // Procura se o destinatario eh um usuario cadastrado
                            // Neste servidor fake, nao hah lista de usuarios cadastrados
                            // Os unicos outros usuarios cadastrados sao "userfake1" e "userfake2"

                            // Testa se o destinatario eh valido e eh um usuario cadastrado
                            if (!M.setDestinatario(destinatario) ||
                                    testarNomeUsuario(destinatario)) throw 9;
                            // Fixa o indice do usuario FAKE de acordo com o nome
                            // Esse indice nao existe no servidor real
                            /*if (M.getDestinatario()=="userfake1")
                              id_fake = 0;
                            else
                              id_fake = 1;*/

                            // Mensagem valida
                            // Insere a mensagem no buffer
                            // Este servidor fake nao tem um buffer de verdade
                            // Apenas guarda, para simulacao, a ultima msg recebida
                            // (do usuario para userfake1 e para userfake2)
                            // No servidor real, a mensagem teria que ser inserida (push_back)
                            // na lista de mensagens
                            //doUsuario[id_fake] = M;
                            listMsg.push_back(M);
                            iMsg = listMsg.end();
                            iMsg--;
                            // Atualiza a ultima ID recebida
                            iUser->setLastId(id);

                            // Envia a confirmacao de recebimento
                            if (iUser->write_int(CMD_MSG_RECEBIDA) != mysocket_status::SOCK_OK) throw 10;
                            if (iUser->write_int(id) != mysocket_status::SOCK_OK) throw 11;

                            // Confirmacao de recebimento enviada
                            imprimeComandoEnviado(iUser->getLogin(), CMD_MSG_RECEBIDA, id);

                            // Testa se o destinatario estah conectado
                            // Se sim, envia a mensagem e muda status para MSG_ENTREGUE
                            /* **************************************************************************************
                            ATENCAO: a parte a seguir da implementacao do servidor FAKE ***NAO*** pode ser adaptada
                            para o servidor real, a nao ser que seja feita uma analise muito cuidadosa.
                            ************************************************************************************** */
                            // No servidor real, deveria utilizar a funcao auxiliar enviarMensagem
                            //
                            // Neste servidor fake, userfake1 e userfake2 estao sempre conectados
                            // e os envios para eles sempre dao certo...
                            // Vamos fazer aqui mesmo as operacoes que, no servidor real, seriam
                            // executadas na funcao auxiliar enviarMensagem:
                            // a) informa que foi "enviada" a mensagem para o remetente fake
                            imprimeComandoEnviado(iMsg->getDestinatario(), CMD_NOVA_MSG, iMsg->getId(), iMsg->getRemetente());
                            // b) Mensagem "enviada": atualiza status no "buffer"
                            iMsg->setStatus(MsgStatus::MSG_ENTREGUE);
                            listMsg.push_back(*iMsg); //estudo de buffer; resolvido talvez
                            // c) Testa se o remetente estah conectado (normalmente sempre estah)
                            // Se sim, envia a confirmacao de entrega da mensagem
                            if (iUser->connected())
                            {
                                enviarConfirmacao(iMsg, iUser);
                            }
                            // Fim da parte que seria feita pela funcao auxiliar enviarMensagem
                            //
                            /* Fim da parte que ***NAO*** pode ser adaptada para o servidor real ***************** */
                            break; // Fim do case CMD_NOVA_MSG
                        case CMD_LOGOUT_USER:
                            imprimeComandoRecebido(iUser->getLogin(), CMD_LOGOUT_USER);
                            iUser->close();
                            break; // Fim do case CMD_LOGOUT_USER
                        } // Fim do switch(cmd)
                    } // Fim do if (had_activity) no socket do cliente
                } // Fim do try para erros nos clientes
                catch (int erro) // Erros na leitura do socket de algum cliente
                {
                    if (erro>=6 && erro<=9)
                    {
                        // Comunicacao OK, mensagem invalida
                        // Envia comando informando msg mal formatada
                        iUser->write_int(CMD_MSG_INVALIDA);
                        iUser->write_int(id);

                        // Comando de mensagem invalida enviado
                        imprimeComandoEnviado(iUser->getLogin(), CMD_MSG_INVALIDA, id, destinatario);
                    }
                    else // erro 1-6 e 10-11
                    {
                        // Erro na comunicacao
                        // Fecha o socket
                        iUser->close();

                        // Informa o erro imprevisto
                        cerr << "Erro " << erro << " na leitura de nova mensagem do cliente "
                             << iUser->getLogin() << endl;
                    }
                }

                // Depois de testar os sockets dos clientes,
                // testa se houve atividade no socket de conexao
                if (f.had_activity(c))
                {
                    tcp_mysocket t;
                    string login, senha;

                    // Aceita provisoriamente a nova conexao
                    if (c.accept(t) != mysocket_status::SOCK_OK) throw 3; // Erro grave: encerra o servidor

                    try // Erros na conexao de cliente: fecha socket temporario ou desconecta novo cliente
                    {
                        // Leh o comando
                        iResult = t.read_int(cmd, TIMEOUT_LOGIN_WHATSPROG*1000);
                        if (iResult != mysocket_status::SOCK_OK) throw 1;

                        // Testa o comando
                        if (cmd!=CMD_LOGIN_USER && cmd!=CMD_NEW_USER) throw 2;

                        // Leh o parametro com o login do usuario que deseja se conectar
                        iResult = t.read_string(login, TIMEOUT_LOGIN_WHATSPROG*1000);
                        if (iResult != mysocket_status::SOCK_OK) throw 3;

                        // Leh o parametro com a senha do usuario que deseja se conectar
                        iResult = t.read_string(senha, TIMEOUT_LOGIN_WHATSPROG*1000);
                        if (iResult != mysocket_status::SOCK_OK) throw 4;

                        // Nova tentativa de conexao recebida
                        imprimeComandoRecebido(login, (ComandoWhatsProg)cmd);

                        // Testa o login e senha
                        if (!testarNomeUsuario(login) || !testarSenha(senha)) throw 5;

                        // Verifica se jah existe um usuario cadastrado com esse login
                        // No servidor real, deveria ser feita uma busca na lista de usuarios
                        // Aqui, basta comparar com os nomes "userfake1" ou "userfake2"
                        // nos testes a seguir sobre se o usuario eh adequado ou nao

                        // Testa se o usuario eh adequado
                        /* **************************************************************************************
                        ATENCAO: a parte a seguir da implementacao do servidor FAKE pode ser parcialmente adaptada
                        para o servidor real, mas requer uma analise cuidadosa.
                        ************************************************************************************** */
                        if (cmd == CMD_NEW_USER)
                        {
                            if (login == iUser->getLogin()) throw 6; // Erro se jah existir

                            // Este servidor fake soh pode ter um usuario cadastrado
                            // Soh vai aceitar o novo usuario se ninguem estiver conectado
                            // Nao eh um erro no servidor real: pode se conectar um novo se
                            // jah houver um ou varios conectados
                            if (iUser->connected()) throw 6;

                            // Insere novo usuario
                            //
                            // No servidor fake, soh pode ter um usuario cadastrado
                            // Portanto, ele vai sobrescrever um eventual usuario previamente cadastrado
                            // Esse usuario anterior, caso tente futuramente se conectar com login jah
                            // cadastrado, nao serah reconhecido

                            Usuario novo;

                            //fazer: setar informações e adicionar na lista;

                            // Troca o usuario
                            if (!iUser->setUsuario(login,senha)) throw 7;
                            //iUser->resetId(); // Soh no FAKE
                            iUser->swapSocket(t);
                            // No servidor real, deveria inserir o novo usuario na lista de usuarios
                            listUser.push_front(novo);
                        }
                        else  // else cmd == CMD_NEW_USER; implica cmd eh CMD_LOGIN_USER
                        {
                            if (login != iUser->getLogin()) throw 8; // Erro se nao existir

                            // Testa se a senha confere
                            if (!iUser->validarSenha(senha)) throw 9; // Senha nao confere
                            // Testa se o cliente jah estah conectado
                            if (iUser->connected()) throw 10; // Usuario jah conectado
                            // Associa o socket que se conectou a um usuario cadastrado
                            iUser->swapSocket(t);
                        } // fim cmd eh CMD_LOGIN_USER
                        /* Fim da parte que pode ser PARCIALMENTE adaptada para o servidor real ************** */

                        // Envia a confirmacao de conexao para o novo cliente
                        if (iUser->write_int(CMD_LOGIN_OK) != mysocket_status::SOCK_OK) throw 11;
                        imprimeComandoEnviado(iUser->getLogin(), CMD_LOGIN_OK);

                        // Se for um cliente antigo, envia para o cliente que se reconectou:
                        // a) as mensagens enviadas para ele que estao no buffer
                        // b) as confirmacoes de visualizacao para ele que estao no buffer
                        if (cmd==CMD_LOGIN_USER)
                        {
                            /* **************************************************************************************
                            ATENCAO: a parte a seguir da implementacao do servidor FAKE ***NAO*** pode ser adaptada
                            para o servidor real, a nao ser que seja feita uma analise muito cuidadosa.
                            ************************************************************************************** */
                            // mensagens enviadas para ele que estao no buffer
                            //
                            // No servidor real, deveria ser feita uma busca para localizar todas as
                            // mensagens enviadas para ele que estao no buffer como recebidas
                            // No servidor FAKE, basta verificar as eventuais 2 msgs que estao em paraUsuario[i]
                            for (int i=0; i<2; ++i)
                            {
                                //if (listMsg.front().getStatus() ==  MsgStatus::MSG_RECEBIDA &&
                                //        listMsg.front().getDestinatario() == iUser->getLogin())
                                //{
                                // No servidor real, essa funcao auxiliar teria que receber como
                                // parametro um iterador para a mensagem a ser enviada e
                                // um iterador para o usuario que se conectou
                                enviarMensagem(iMsg, iUser);
                                //} // fim if (paraUsuario[i].getStatus() == ...
                            } // fim for (int i=0; i<2; ++i)
                            // as confirmacoes de entrega para ele que estao no buffer
                            //
                            // No servidor real, deveria ser feita uma busca para localizar todas as
                            // mensagens enviadas por ele que estao no buffer como entregues
                            // No servidor FAKE, basta verificar as eventuais 2 msgs que estao em doUsuario[i]
                            for (int i=0; i<2; ++i)
                            {
                                //if (listMsg.front().getStatus() ==  MsgStatus::MSG_ENTREGUE &&
                                //        listMsg.front().getRemetente() == iUser->getLogin())
                                //{
                                // No servidor real, essa funcao auxiliar teria que receber como
                                // parametro um iterador para a mensagem a ser enviada a confirmacao e
                                // um iterador para o usuario que se conectou
                                enviarConfirmacao(iMsg, iUser);
                                // } // fim if (doUsuario[i].getStatus() == ...
                            } // fim for (int i=0; i<2; ++i)
                        } // fim do if (cmd==CMD_LOGIN_USER)
                    } // Fim do try para erros na comunicacao com novo cliente
                    catch (int erro) // Erros na conexao do novo cliente
                    {
                        if (erro>=5 && erro<=10)
                        {
                            // Comunicacao com socket temporario OK, login invalido
                            // Envia comando informando login invalido
                            t.write_int(CMD_LOGIN_INVALIDO);

                            // Comando login invalido enviado
                            imprimeComandoEnviado(login, CMD_LOGIN_INVALIDO);

                            // Fecha o socket temporario
                            t.close();
                        }
                        else // erro 1-4 ou 11
                        {
                            // Erro na comunicacao
                            // Fecha o socket
                            if (erro==11)
                            {
                                // Erro na comunicacao com socket do novo cliente
                                iUser->close();
                            }
                            else  // erro 1-4
                            {
                                // Erro na comunicacao com socket temporario
                                t.close();
                            }
                            // Informa erro nao previsto
                            cerr << "Erro " << erro << " na conexao de novo cliente" << endl;
                        }
                    } // fim catch
                } // fim if (had_activity) no socket de conexoes
                break; // fim do case mysocket_status::SOCK_OK - resultado do wait_read
            } // fim do switch (iResult) - resultado do wait_read

        } // Fim do try para erros criticos no servidor
        catch (int erro) // Erros criticos no servidor
        {
            cerr << "Erro " << erro << " no servidor. Encerrando\n";
            fim = true;
        }

    } // Fim do while (!fim), laco principal da thread

    cout << "\nServidor encerrado.\n";

    return 0;
}
