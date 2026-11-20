#pragma once
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

// ---------------------------------------------------------------
// CLASE PRINCIPAL (abstracta): lo que tienen en comun todas las actividades.
// ENCAPSULAMIENTO: titulo_ y puntaje_ son privados; solo se leen con
// getters y el puntaje solo puede subir con sumarPuntos().
// ---------------------------------------------------------------
class Actividad {
private:
    std::string titulo_;
    int puntaje_ = 0;

protected:
    explicit Actividad(std::string titulo) : titulo_(std::move(titulo)) {}
    void sumarPuntos(int puntos) { if (puntos > 0) puntaje_ += puntos; }

public:
    virtual ~Actividad() = default;

    const std::string& titulo() const { return titulo_; }
    int puntaje() const { return puntaje_; }

    virtual void ejecutar() = 0;   // cada actividad la hace a su manera
};

// ---------------------------------------------------------------
// Pregunta: guarda sus datos privados. La respuesta correcta NO tiene
// getter: solo se puede comprobar con esCorrecta().
// ---------------------------------------------------------------
class Pregunta {
private:
    std::string enunciado_;
    std::vector<std::string> opciones_;
    int correcta_;                 // indice base 0
    std::string explicacion_;

public:
    Pregunta(std::string enunciado, std::vector<std::string> opciones,
             int correcta, std::string explicacion)
        : enunciado_(std::move(enunciado)), opciones_(std::move(opciones)),
          correcta_(correcta), explicacion_(std::move(explicacion)) {}

    const std::string& enunciado() const { return enunciado_; }
    const std::vector<std::string>& opciones() const { return opciones_; }
    const std::string& explicacion() const { return explicacion_; }
    bool esCorrecta(int opcion) const { return opcion == correcta_; }
};

// ---------------------------------------------------------------
// HERENCIA 1: Cuestionario "es una" Actividad
// ---------------------------------------------------------------
class Cuestionario : public Actividad {
private:
    std::vector<Pregunta> preguntas_;
    std::vector<int> respuestas_;  // lo que eligio el usuario

    static int leerOpcion(std::size_t total) {
        int op = 0;
        while (true) {
            std::cout << "Tu respuesta (1-" << total << "): ";
            if (std::cin >> op) {
                if (op >= 1 && op <= static_cast<int>(total)) return op - 1;
                std::cout << "Opcion fuera de rango.\n";
            } else if (std::cin.eof()) {
                return -1;         // no hay mas entrada
            } else {
                std::cin.clear();
                std::cin.ignore(1000, '\n');
                std::cout << "Escribe solo un numero.\n";
            }
        }
    }

public:
    explicit Cuestionario(std::string titulo) : Actividad(std::move(titulo)) {}

    void agregar(Pregunta pregunta) { preguntas_.push_back(std::move(pregunta)); }

    void ejecutar() override {
        std::cout << "\n=== " << titulo() << " ===\n";
        respuestas_.clear();
        for (const auto& p : preguntas_) {
            std::cout << "\n" << p.enunciado() << "\n";
            for (std::size_t i = 0; i < p.opciones().size(); ++i)
                std::cout << "  " << i + 1 << ") " << p.opciones()[i] << "\n";

            int opcion = leerOpcion(p.opciones().size());
            respuestas_.push_back(opcion);

            if (p.esCorrecta(opcion)) { sumarPuntos(1); std::cout << "Correcto. "; }
            else                      { std::cout << "No exactamente. "; }
            std::cout << p.explicacion() << "\n";
        }
        std::cout << "\nAciertos: " << puntaje() << " de " << preguntas_.size() << "\n";
    }
};

// ---------------------------------------------------------------
// HERENCIA 2: Simulacion "es una" Actividad
// Modelo: cache = 1 ns, RAM = 80 ns, la CPU calcula 5 ns por cada dato.
// ---------------------------------------------------------------
class Simulacion : public Actividad {
private:
    double tasaCache_ = 0.9;       // fraccion de veces que el dato esta en cache
    static constexpr double LATENCIA_CACHE_NS = 1.0;
    static constexpr double LATENCIA_RAM_NS = 80.0;
    static constexpr double TRABAJO_NS = 5.0;

    double tiempoPorDatoNs() const {
        return LATENCIA_CACHE_NS + (1.0 - tasaCache_) * LATENCIA_RAM_NS;
    }
    double porcentajeUtil() const {
        return TRABAJO_NS / (TRABAJO_NS + tiempoPorDatoNs()) * 100.0;
    }

public:
    explicit Simulacion(std::string titulo) : Actividad(std::move(titulo)) {}

    // El setter valida el valor: asi el objeto nunca queda en un estado invalido
    void ajustarTasaCache(double tasa) { tasaCache_ = std::clamp(tasa, 0.0, 1.0); }

    void ejecutar() override {
        std::cout << "\n=== " << titulo() << " ===\n";
        std::cout << std::fixed << std::setprecision(1);
        for (double tasa : {0.0, 0.5, 0.9, 1.0}) {
            ajustarTasaCache(tasa);
            int barras = static_cast<int>(porcentajeUtil() / 5.0);
            std::cout << "Aciertos de cache " << std::setw(3) << static_cast<int>(tasa * 100)
                      << " % | " << std::setw(5) << tiempoPorDatoNs() << " ns por dato | CPU trabaja "
                      << std::setw(5) << porcentajeUtil() << " % " << std::string(barras, '#') << "\n";
        }
        sumarPuntos(1);            // completar la simulacion cuenta como 1 punto
    }
};
