
#include "ModernDreamClient.h"
#include <QApplication>
#include "LoginDialog.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv); 

    LoginDialog loginDialog;
    loginDialog.show();

    return app.exec(); 
    

}