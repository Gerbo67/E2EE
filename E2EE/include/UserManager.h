#pragma once

#include "User.h"
#include <vector>
#include <string>
#include <unordered_map>
#include <mutex>
#include <memory>
#include <optional>

/**
 * @class UserManager
 * @brief Gestiona el ciclo de vida y las operaciones de los usuarios del chat.
 *
 * Esta clase (implementada como un Singleton) es responsable de:
 * - Añadir y eliminar usuarios.
 * - Asignarles un nombre de usuario único.
 * - Almacenar y proporcionar su información pública (nombre y clave pública).
 * - Difundir mensajes a los usuarios conectados.
 */
class UserManager {
public:
    /**
     * @struct UserInfo
     * @brief Contiene la información pública de un usuario para ser compartida.
     */
    struct UserInfo {
        std::string username;
        std::string publicKey;
    };

    /**
     * @brief Obtiene la instancia única del UserManager (Patrón Singleton).
     */
    static UserManager& getInstance();

    UserManager(const UserManager&) = delete;
    void operator=(const UserManager&) = delete;

    /**
     * @brief Registra un nuevo usuario y le asigna un nombre.
     */
    std::shared_ptr<User> addUser(crow::websocket::connection* conn);
    
    /**
     * @brief Elimina un usuario basado en su conexión.
     */
    std::optional<std::string> removeUser(crow::websocket::connection* conn);
    
    /**
     * @brief Busca un usuario por su nombre.
     */
    std::shared_ptr<User> findUserByUsername(const std::string& username) const;
    
    /**
     * @brief Busca un usuario por su conexión.
     */
    std::shared_ptr<User> findUserByConnection(crow::websocket::connection* conn) const;
    
    /**
     * @brief Obtiene la información (nombre y clave pública) de todos los usuarios.
     * @return Un vector de estructuras UserInfo.
     */
    std::vector<UserInfo> getAllUsersInfo() const;

    /**
     * @brief Envía un mensaje a todos los usuarios, con opción de excluir a uno.
     */
    void broadcast(const std::string& message, crow::websocket::connection* exclude_conn = nullptr) const;

private:
    UserManager();
    std::string generateUniqueUsername();

    mutable std::mutex mtx;
    std::unordered_map<crow::websocket::connection*, std::shared_ptr<User>> users_by_conn;
    std::unordered_map<std::string, std::shared_ptr<User>> users_by_name;
    std::vector<std::string> base_names;
};
