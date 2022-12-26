#include "whatsprogthread.h"
#include "whatsprog_dados_cliente.h"
#include "whatsprog_dados.h"

extern WhatsProgDadosCliente DC;
extern tcp_mysocket sock;

WhatsProgThread::WhatsProgThread(QObject *parent)
    : QObject{parent}
{

}

void WhatsProgThread::main_thread()
{
    mysocket_status iResult;

    while (sock.connected())
    {
        int32_t cmd;
        iResult = sock.read_int(cmd,1000*TIMEOUT_WHATSPROG);
        if (iResult == mysocket_status::SOCK_OK)
        {
            iterConversa it;
            int32_t id;

            switch(cmd)
            {
            case CMD_NEW_USER:
            case CMD_LOGIN_USER:
            case CMD_LOGIN_OK:
            case CMD_LOGIN_INVALIDO:

            case CMD_LOGOUT_USER:
            default:
                break;
            case CMD_NOVA_MSG:
            {
                std::string remetente;
                std::string texto;

                iResult = sock.read_int(id, TIMEOUT_WHATSPROG*1000);
                if (iResult == mysocket_status::SOCK_OK) iResult = sock.read_string(remetente, TIMEOUT_WHATSPROG*1000);
                if (iResult == mysocket_status::SOCK_OK) iResult = sock.read_string(texto, TIMEOUT_WHATSPROG*1000);
                if (iResult == mysocket_status::SOCK_OK)
                {
                    it = DC.findConversa(remetente);
                    if (it == DC.end())
                    {
                        if (DC.insertConversa(remetente))
                        {
                            it = DC.last();
                        }
                    }
                    if (it != DC.end())
                    {
                        Mensagem M;
                        if (M.setId(id) && M.setRemetente(remetente) && M.setDestinatario(DC.getMeuUsuario()) &&
                                M.setTexto(texto) && M.setStatus(MsgStatus::MSG_ENTREGUE))
                        {
                            DC.pushMessage(it,M);
                            DC.moveConversaToBegin(it);
                        }
                    }
                }
                else
                {
                    sock.close();
                }
                emit novaMensagem(it);
                break;
            }
            case CMD_MSG_RECEBIDA:
            case CMD_MSG_ENTREGUE:
            case CMD_MSG_INVALIDA:
            {
                int ind_msg;
                iResult = sock.read_int(id, TIMEOUT_WHATSPROG*1000);
                if (iResult == mysocket_status::SOCK_OK)
                {
                    DC.findMensagem(id, it, ind_msg);
                    if (it!=DC.end() && ind_msg>=0)
                    {
                        if (it->getMensagem(ind_msg).getRemetente()==DC.getMeuUsuario() &&
                                it->getMensagem(ind_msg).getStatus()==MsgStatus::MSG_ENVIADA)
                        {
                            DC.eraseMessage(it,ind_msg);
                        }
                        else
                        {
                        }
                    }
                    else
                    {
                    }
                }
                else
                {
                    sock.close();
                }
                break;
            }
            }
        }
        else
        {
            if (iResult == mysocket_status::SOCK_TIMEOUT)
            {
                if (!DC.salvar())
                {
                }
            }
            else
            {
                sock.close();
            }
        }
    }
    if (sock.connected())
    {
        sock.write_int(CMD_LOGOUT_USER);
    }
    sock.close();
    DC.salvar();
}

void main_thread(WhatsProgThread *_thr)
{
    _thr->main_thread();
}

void WhatsProgThread::confirmarThread()
{
    thr = std::thread(::main_thread, this);
}

void WhatsProgThread::finalizarThread()
{
    if (sock.connected())
    {
        sock.write_int(CMD_LOGOUT_USER);
    }
    sock.close();
    DC.unsetServidorUsuario();

    if(thr.joinable())
        thr.join();

    emit desconectar();
}
