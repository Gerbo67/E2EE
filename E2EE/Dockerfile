# =========================================================================
# FASE 1: CONSTRUCTOR (BUILDER)
# =========================================================================
FROM debian:bookworm AS builder

# Instalar dependencias de compilación: g++, make y las librerías de OpenSSL
RUN apt-get update && \
    apt-get install -y g++ make libssl-dev

# Establecer el directorio de trabajo dentro del contenedor
WORKDIR /app

# Copiar el código fuente y los headers al contenedor
COPY src/ ./src/
COPY include/ ./include/

# Compilar la aplicación para producción
# -std=c++17 : Usa el estándar C++17
# -O3        : Máxima optimización para velocidad
# -Wall      : Muestra todas las advertencias (buena práctica)
# -o E2EE    : Nombre del archivo ejecutable de salida
# -I./include: Directorio de headers
# -l...      : Enlaza las librerías necesarias
RUN g++ -std=c++17 -O3 -Wall -o E2EE src/*.cpp -I./include -lpthread -lssl -lcrypto

# Copiar los archivos estáticos que servirá la API
COPY public/ ./public/


# =========================================================================
# FASE 2: IMAGEN FINAL
# =========================================================================
FROM debian:bookworm-slim

# Instalar solo las dependencias de EJECUCIÓN (no las de desarrollo)
RUN apt-get update && \
    apt-get install -y libssl3 && \
    rm -rf /var/lib/apt/lists/*

# Establecer el directorio de trabajo
WORKDIR /app

# Copiar ÚNICAMENTE el ejecutable compilado y los archivos estáticos desde la fase "builder"
COPY --from=builder /app/E2EE .
COPY --from=builder /app/public ./public/

# Exponer el puerto que usa la aplicación. Es una buena práctica documentarlo.
EXPOSE 18080

# Definir la variable de entorno PORT. Tu código la leerá.
ENV PORT=18080

# Comando que se ejecutará cuando inicie el contenedor
CMD ["./E2EE"]