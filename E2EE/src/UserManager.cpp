#include "UserManager.h"
#include <random>
#include <algorithm>
#include <ctime>

/**
 * @summary Constructor privado que inicializa la lista de nombres base.
 */
UserManager::UserManager() {
    base_names = {"GatoOculto", "PerroSigiloso", "LoboSolitario", "AguilaAudaz", "ZorroAstuto", "OsoValiente", "DragonMistico", "SerpienteSabia", "PanteraNegra", "CiervoVeloz"};
    srand(time(0));
}

/**
 * @summary Obtiene la instancia única de la clase (Singleton).
 */
UserManager& UserManager::getInstance() {
    static UserManager instance;
    return instance;
}

/**
 * @summary Genera un nombre de usuario único y aleatorio.
 */
std::string UserManager::generateUniqueUsername() {
    std::string username;
    do {
        std::string base = base_names[rand() % base_names.size()];
        int number = rand() % 9000 + 1000;
        username = base + std::to_string(number);
    } while (users_by_name.count(username));
    return username;
}

/**
 * @summary Añade un nuevo usuario al sistema.
 */
std::shared_ptr<User> UserManager::addUser(crow::websocket::connection* conn) {
    std::lock_guard<std::mutex> lock(mtx);
    std::string username = generateUniqueUsername();
    auto user = std::make_shared<User>(conn, username);
    users_by_conn[conn] = user;
    users_by_name[username] = user;
    return user;
}

/**
 * @summary Elimina un usuario del sistema a partir de su conexión.
 */
std::optional<std::string> UserManager::removeUser(crow::websocket::connection* conn) {
    std::lock_guard<std::mutex> lock(mtx);
    auto it_conn = users_by_conn.find(conn);
    if (it_conn != users_by_conn.end()) {
        std::string username = it_conn->second->getUsername();
        users_by_name.erase(username);
        users_by_conn.erase(it_conn);
        return username;
    }
    return std::nullopt;
}

/**
 * @summary Busca un usuario por su nombre.
 */
std::shared_ptr<User> UserManager::findUserByUsername(const std::string& username) const {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = users_by_name.find(username);
    if (it != users_by_name.end()) {
        return it->second;
    }
    return nullptr;
}

/**
 * @summary Busca un usuario por su conexión.
 */
std::shared_ptr<User> UserManager::findUserByConnection(crow::websocket::connection* conn) const {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = users_by_conn.find(conn);
    if (it != users_by_conn.end()) {
        return it->second;
    }
    return nullptr;
}

/**
 * @summary Devuelve la información pública de todos los usuarios registrados.
 */
std::vector<UserManager::UserInfo> UserManager::getAllUsersInfo() const {
    std::lock_guard<std::mutex> lock(mtx);
    std::vector<UserInfo> users_info;
    users_info.reserve(users_by_name.size());
    for (const auto& pair : users_by_name) {
        if (!pair.second->getPublicKey().empty()) {
            users_info.push_back({pair.first, pair.second->getPublicKey()});
        }
    }
    return users_info;
}

/**
 * @summary Envía un mensaje a todos los usuarios, opcionalmente excluyendo al remitente.
 */
void UserManager::broadcast(const std::string& message, crow::websocket::connection* exclude_conn) const {
    std::lock_guard<std::mutex> lock(mtx);
    for (const auto& pair : users_by_conn) {
        if (pair.first != exclude_conn && !pair.second->getPublicKey().empty()) {
            pair.second->sendMessage(message);
        }
    }
}
