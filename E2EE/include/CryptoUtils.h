#pragma once

#include <string>
#include <vector>

/**
 * @class CryptoUtils
 * @brief Proporciona funciones de utilidad para operaciones criptográficas.
 *
 * Esta clase contiene métodos estáticos para tareas comunes como la
 * codificación y decodificación Base64, necesarias para transmitir
 * datos binarios en formatos de texto como JSON.
 */
class CryptoUtils {
public:
    /**
     * @brief Codifica datos binarios a una cadena Base64.
     * @param data Vector de bytes a codificar.
     * @return Una cadena de texto en formato Base64.
     */
    static std::string base64_encode(const std::vector<unsigned char>& data);

    /**
     * @brief Decodifica una cadena Base64 a datos binarios.
     * @param input La cadena en formato Base64.
     * @return Un vector de bytes con los datos decodificados.
     */
    static std::vector<unsigned char> base64_decode(const std::string& input);
};
