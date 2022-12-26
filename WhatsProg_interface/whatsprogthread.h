#ifndef WHATSPROGTHREAD_H
#define WHATSPROGTHREAD_H

#include <QObject>
#include <thread>

#include"whatsprog_dados_cliente.h"

class WhatsProgThread : public QObject
{
    Q_OBJECT
public:
    explicit WhatsProgThread(QObject *parent = nullptr);

signals:
    void novaMensagem(iterConversa itr);
    void confirmacaoLeitura(iterConversa itr);
    void fimThread();
    void desconectar();

private:
    std::thread thr;
    void main_thread(void);
    friend void main_thread(WhatsProgThread *_thr);

private slots:
    void confirmarThread();
    void finalizarThread();
};

#endif // WHATSPROGTHREAD_H
