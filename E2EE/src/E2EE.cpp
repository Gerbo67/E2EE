#define _CRT_SECURE_NO_WARNINGS
#define CROW_MAIN

#include "crow.h"
#include "UserManager.h"
#include "MessageHandler.h"
#include <iostream>
#include <filesystem>

int main() {
    std::cout << "Directorio de trabajo actual: " << std::filesystem::current_path() << std::endl;
    
    crow::SimpleApp app;
    auto& userManager = UserManager::getInstance();

    // Sirve el archivo HTML principal
    CROW_ROUTE(app, "/")
    ([](const crow::request&, crow::response& res) {
        res.set_static_file_info("public/index.html");
        res.end();
    });

    // Maneja las conexiones WebSocket
    CROW_ROUTE(app, "/ws")
        .websocket(&app)
        .onopen([&](crow::websocket::connection& conn) {
            CROW_LOG_INFO << "Nueva conexión WebSocket: " << &conn;
            // Añade al usuario al gestor
            auto new_user = userManager.addUser(&conn);
            // Inicia el handshake pidiendo al cliente su clave pública
            new_user->sendMessage(MessageHandler::buildKeyRequestMessage());
        })
        .onclose([&](crow::websocket::connection& conn, const std::string& reason, uint16_t code) {
            CROW_LOG_INFO << "Conexión cerrada: " << &conn;
            auto user = userManager.findUserByConnection(&conn);
            if (user && !user->getPublicKey().empty()) {
                // Si el usuario se había registrado completamente, notificar a los demás
                userManager.broadcast(MessageHandler::buildUserStatusUpdate(user->getUsername(), "", "offline"), &conn);
            }
            // Eliminar al usuario del gestor
            userManager.removeUser(&conn);
        })
        .onmessage([&](crow::websocket::connection& conn, const std::string& data, bool is_binary) {
            if (is_binary) return;
            // Delegar todo el procesamiento de mensajes al MessageHandler
            MessageHandler::process(conn, data);
        });

    // Configura y ejecuta el servidor
    char* port_str = getenv("PORT");
    int port = port_str ? std::stoi(port_str) : 18080;

    CROW_LOG_INFO << "Servidor de Chat Cifrado iniciado en el puerto " << port;
    app.port(port).multithreaded().run();

    return 0;
}
