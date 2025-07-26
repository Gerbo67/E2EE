#include "User.h"

/**
 * @summary Constructor que inicializa un usuario con su conexión y nombre.
 */
User::User(crow::websocket::connection* conn, std::string name)
    : connection(conn), username(std::move(name)) {}

/**
 * @summary Envía un mensaje de texto a través de la conexión WebSocket del usuario.
 */
void User::sendMessage(const std::string& message) const {
    if (connection) {
        connection->send_text(message);
    }
}

/**
 * @summary Devuelve el nombre de usuario.
 */
std::string User::getUsername() const {
    return username;
}

/**
 * @summary Devuelve el puntero a la conexión del usuario.
 */
crow::websocket::connection* User::getConnection() const {
    return connection;
}

/**
 * @summary Almacena la clave pública del usuario.
 */
void User::setPublicKey(std::string key) {
    publicKey = std::move(key);
}

/**
 * @summary Devuelve la clave pública del usuario.
 */
std::string User::getPublicKey() const {
    return publicKey;
}
