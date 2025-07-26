#pragma once

#include "crow.h"
#include <string>

/**
 * @class User
 * @brief Representa a un usuario conectado al chat con su clave pública.
 *
 * Almacena la información de conexión, el nombre de usuario y la clave
 * pública en formato PEM, que es esencial para el cifrado de extremo a extremo.
 */
class User {
public:
    /**
     * @brief Constructor de la clase User.
     * @param conn Puntero a la conexión WebSocket del usuario.
     * @param name Nombre de usuario único asignado.
     */
    User(crow::websocket::connection* conn, std::string name);

    /**
     * @brief Envía un mensaje de texto a este usuario.
     * @param message El mensaje a enviar.
     */
    void sendMessage(const std::string& message) const;

    /**
     * @brief Obtiene el nombre de usuario.
     * @return El nombre de usuario como std::string.
     */
    std::string getUsername() const;

    /**
     * @brief Obtiene el puntero a la conexión WebSocket.
     * @return Puntero a la conexión crow::websocket::connection.
     */
    crow::websocket::connection* getConnection() const;

    /**
     * @brief Asigna la clave pública (formato PEM) a este usuario.
     * @param key La clave pública como string.
     */
    void setPublicKey(std::string key);

    /**
     * @brief Obtiene la clave pública del usuario.
     * @return La clave pública en formato PEM.
     */
    std::string getPublicKey() const;

private:
    crow::websocket::connection* connection; ///< Puntero a la conexión activa del usuario.
    std::string username;                    ///< Nombre de usuario único.
    std::string publicKey;                   ///< Clave pública del usuario en formato PEM.
};
