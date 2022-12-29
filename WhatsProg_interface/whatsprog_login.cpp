#include "whatsprog_login.h"
#include "ui_whatsprog_login.h"
#include "whatsprog_dados.h"
#include <QMessageBox>
///Autores: Marcos Paulo Barbosa && Luisa de Moura Galvao Mathias
WhatsProgLogin::WhatsProgLogin(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::WhatsProgLogin),
  novoUsuario(false)
{
  ui->setupUi(this);

  ui->lineEditSenhaUsuario->setEchoMode( QLineEdit::Password );
}

WhatsProgLogin::~WhatsProgLogin()
{
  delete ui;
}

void WhatsProgLogin::slotLogin(bool NovoUsuario)
{
  novoUsuario = NovoUsuario;
  if (novoUsuario) setWindowTitle("Usuario - Criar");
  else setWindowTitle("Usuario - Conectar");
  ui->lineEditIpServidor->clear();
  ui->lineEditNomeUsuario->clear();
  ui->lineEditSenhaUsuario->clear();
  show();
}

void WhatsProgLogin::on_buttonBox_accepted()
{
    QString IP, usuario, senha;

    IP = ui->lineEditIpServidor->text();
    usuario = ui->lineEditNomeUsuario->text();
    senha = ui->lineEditSenhaUsuario->text();

    if(testarNomeUsuario(usuario.toStdString()) && testarSenha(senha.toStdString()))
    {
        emit signConectar(IP, usuario, senha, novoUsuario);
        ui->lineEditIpServidor->clear();
        ui->lineEditNomeUsuario->clear();
        ui->lineEditSenhaUsuario->clear();
    }
    else QMessageBox::critical(this, "Erro ao realizar o login", "Dados inválidos");
  // Envia sinal com os dados recuperados dos lineEdit e limpa os campos dos lineEdit
}
