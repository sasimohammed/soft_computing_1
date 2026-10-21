#include "cvrp/CVRPParser.h"
#include <algorithm>
#include <cctype>
#include <cmath>
#include <fstream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <vector>

namespace cvrp {
namespace {

std::string trim(const std::string& s) {
    const auto begin = s.find_first_not_of(" \t\r\n");
    if (begin == std::string::npos) return "";
    const auto end = s.find_last_not_of(" \t\r\n");
    return s.substr(begin, end - begin + 1);
}

std::string upper(std::string s) {
    for (char& c : s) c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    return s;
}

class Parser {
public:
    Parser(std::istream& in, std::string source) : in_(in), source_(std::move(source)) {}

    CVRPProblem parse() {
        readFile();
        return build();
    }

private:
    enum class Section { None, Coords, Demands, Depots, Weights, Ignore };

    [[noreturn]] void fail(const std::string& message, std::size_t line = 0) const {
        std::string where = source_;
        if (line > 0) where += ":" + std::to_string(line);
        throw std::runtime_error(where + ": " + message);
    }

    double toNumber(const std::string& token, std::size_t line) const {
        try {
            std::size_t used = 0;
            const double v = std::stod(token, &used);
            if (used != token.size() || !std::isfinite(v)) throw std::invalid_argument(token);
            return v;
        } catch (const std::exception&) {
            fail("expected a number but found '" + token + "'", line);
        }
    }

    long toLabel(const std::string& token, std::size_t line) const {
        const double v = toNumber(token, line);
        if (v != std::floor(v)) fail("node label must be an integer: '" + token + "'", line);
        return static_cast<long>(v);
    }

    void readFile() {
        std::string raw;
        std::size_t lineNo = 0;
        Section section = Section::None;
        while (std::getline(in_, raw)) {
            ++lineNo;
            const std::string line = trim(raw);
            if (line.empty()) continue;

            // keyword = first word, with an optional trailing ':'
            std::string keyword = line.substr(0, line.find_first_of(" \t:"));
            keyword = upper(keyword);

            if (keyword == "EOF") break;
            if (keyword == "NODE_COORD_SECTION") { section = Section::Coords; continue; }
            if (keyword == "DEMAND_SECTION") { section = Section::Demands; continue; }
            if (keyword == "DEPOT_SECTION") { section = Section::Depots; continue; }
            if (keyword == "EDGE_WEIGHT_SECTION") { section = Section::Weights; continue; }
            if (keyword == "DISPLAY_DATA_SECTION") { section = Section::Ignore; continue; }

            if (std::isalpha(static_cast<unsigned char>(line[0]))) {  // "KEY : value" header
                const auto colon = line.find(':');
                const std::string value =
                    trim(colon == std::string::npos ? line.substr(keyword.size()) : line.substr(colon + 1));
                header_[keyword] = value;
                headerLine_[keyword] = lineNo;
                section = Section::None;
                continue;
            }

            std::istringstream tokens(line);
            std::vector<std::string> parts;
            for (std::string t; tokens >> t;) parts.push_back(t);

            switch (section) {
                case Section::None:
                    fail("data line outside of any section", lineNo);
                case Section::Ignore:
                    break;
                case Section::Coords: {
                    if (parts.size() != 3) fail("NODE_COORD_SECTION expects 'label x y'", lineNo);
                    const long label = toLabel(parts[0], lineNo);
                    if (coords_.count(label)) fail("duplicate coordinates for node " + parts[0], lineNo);
                    coords_[label] = {toNumber(parts[1], lineNo), toNumber(parts[2], lineNo)};
                    break;
                }
                case Section::Demands: {
                    if (parts.size() != 2) fail("DEMAND_SECTION expects 'label demand'", lineNo);
                    const long label = toLabel(parts[0], lineNo);
                    if (demands_.count(label)) fail("duplicate demand for node " + parts[0], lineNo);
                    const double d = toNumber(parts[1], lineNo);
                    if (d < 0) fail("negative demand for node " + parts[0], lineNo);
                    demands_[label] = d;
                    break;
                }
                case Section::Depots:
                    for (const std::string& p : parts) {
                        const long label = toLabel(p, lineNo);
                        if (label == -1) { section = Section::None; break; }
                        depots_.push_back(label);
                    }
                    break;
                case Section::Weights:
                    for (const std::string& p : parts) weights_.push_back(toNumber(p, lineNo));
                    if (weightsLine_ == 0) weightsLine_ = lineNo;
                    break;
            }
        }
    }

    std::string headerValue(const std::string& key) const {
        auto it = header_.find(key);
        return it == header_.end() ? "" : it->second;
    }
    std::size_t lineOf(const std::string& key) const {
        auto it = headerLine_.find(key);
        return it == headerLine_.end() ? 0 : it->second;
    }

    // Fills a full symmetric matrix from the EDGE_WEIGHT_SECTION numbers.
    std::vector<std::vector<double>> explicitMatrix(std::size_t dim) const {
        const std::string format = upper(headerValue("EDGE_WEIGHT_FORMAT").empty()
                                             ? "FULL_MATRIX" : headerValue("EDGE_WEIGHT_FORMAT"));
        std::vector<std::vector<double>> m(dim, std::vector<double>(dim, 0.0));
        std::size_t expected = 0;
        if (format == "FULL_MATRIX") expected = dim * dim;
        else if (format == "UPPER_ROW" || format == "LOWER_ROW") expected = dim * (dim - 1) / 2;
        else if (format == "UPPER_DIAG_ROW" || format == "LOWER_DIAG_ROW") expected = dim * (dim + 1) / 2;
        else fail("unsupported EDGE_WEIGHT_FORMAT '" + format + "'", lineOf("EDGE_WEIGHT_FORMAT"));
        if (weights_.size() != expected)
            fail("EDGE_WEIGHT_SECTION has " + std::to_string(weights_.size()) + " numbers, " +
                 format + " with DIMENSION " + std::to_string(dim) + " needs " +
                 std::to_string(expected), weightsLine_);

        std::size_t k = 0;
        for (std::size_t i = 0; i < dim; ++i) {
            for (std::size_t j = 0; j < dim; ++j) {
                bool take = false;
                if (format == "FULL_MATRIX") take = true;
                else if (format == "UPPER_ROW") take = j > i;
                else if (format == "LOWER_ROW") take = j < i;
                else if (format == "UPPER_DIAG_ROW") take = j >= i;
                else if (format == "LOWER_DIAG_ROW") take = j <= i;
                if (!take) continue;
                m[i][j] = weights_[k++];
                if (format != "FULL_MATRIX") m[j][i] = m[i][j];
            }
        }
        return m;
    }

    CVRPProblem build() const {
        // ---- scalar headers ---------------------------------------------------------------
        if (headerValue("CAPACITY").empty()) fail("missing CAPACITY");
        const double capacity = toNumber(headerValue("CAPACITY"), lineOf("CAPACITY"));
        if (capacity <= 0) fail("CAPACITY must be positive", lineOf("CAPACITY"));

        const std::string type = upper(headerValue("TYPE"));
        if (!type.empty() && type != "CVRP")
            fail("TYPE must be CVRP (found '" + headerValue("TYPE") + "')", lineOf("TYPE"));

        std::string ewt = upper(headerValue("EDGE_WEIGHT_TYPE"));
        DistanceType distanceType = DistanceType::Euclidean;
        if (ewt.empty() || ewt == "EXACT_2D" || ewt == "FLOAT_2D") distanceType = DistanceType::Euclidean;
        else if (ewt == "EUC_2D") distanceType = DistanceType::RoundedEuclidean;
        else if (ewt == "MAN_2D") distanceType = DistanceType::Manhattan;
        else if (ewt == "EXPLICIT") distanceType = DistanceType::Explicit;
        else fail("unsupported EDGE_WEIGHT_TYPE '" + ewt + "'", lineOf("EDGE_WEIGHT_TYPE"));

        // ---- nodes ------------------------------------------------------------------------
        if (demands_.empty()) fail("missing DEMAND_SECTION");
        std::vector<long> labels;  
        for (const auto& kv : demands_) labels.push_back(kv.first);
        if (!headerValue("DIMENSION").empty()) {
            const double dim = toNumber(headerValue("DIMENSION"), lineOf("DIMENSION"));
            if (dim != static_cast<double>(labels.size()))
                fail("DIMENSION is " + headerValue("DIMENSION") + " but DEMAND_SECTION has " +
                     std::to_string(labels.size()) + " nodes", lineOf("DIMENSION"));
        }
        if (labels.size() < 2) fail("an instance needs a depot and at least one customer");

        if (distanceType != DistanceType::Explicit) {
            if (coords_.empty()) fail("missing NODE_COORD_SECTION");
            for (long label : labels)
                if (!coords_.count(label)) fail("node " + std::to_string(label) + " has no coordinates");
        }
        for (const auto& kv : coords_)
            if (!demands_.count(kv.first)) fail("node " + std::to_string(kv.first) + " has no demand");

        if (depots_.size() > 1) fail("only one depot is supported");
        const long depotLabel = depots_.empty() ? labels.front() : depots_.front();
        if (!demands_.count(depotLabel)) fail("depot " + std::to_string(depotLabel) + " is not a node");
        if (demands_.at(depotLabel) != 0) fail("depot demand must be 0");

        auto coordOf = [&](long label) -> std::pair<double, double> {
            auto it = coords_.find(label);
            return it == coords_.end() ? std::make_pair(0.0, 0.0) : it->second;
        };

       
        std::vector<std::size_t> newToOld; 
        Location depot{0, coordOf(depotLabel).first, coordOf(depotLabel).second, 0.0};
        std::vector<Location> customers;
        for (std::size_t pos = 0; pos < labels.size(); ++pos) {
            if (labels[pos] == depotLabel) { newToOld.insert(newToOld.begin(), pos); continue; }
            const auto xy = coordOf(labels[pos]);
            customers.push_back({static_cast<int>(customers.size()) + 1, xy.first, xy.second,
                                 demands_.at(labels[pos])});
            newToOld.push_back(pos);
        }

        // ---- vehicles ---------------------------------------------------------------------
        int vehicles = 0;
        const std::string name = headerValue("NAME");
        if (!headerValue("VEHICLES").empty()) {
            const double v = toNumber(headerValue("VEHICLES"), lineOf("VEHICLES"));
            if (v < 0 || v != std::floor(v)) fail("VEHICLES must be a non-negative integer", lineOf("VEHICLES"));
            vehicles = static_cast<int>(v);
        } else {  // CVRPLIB naming convention, e.g. "A-n32-k5" -> 5 vehicles
            const auto k = name.rfind("-k");
            if (k != std::string::npos) {
                const std::string digits = name.substr(k + 2);
                if (!digits.empty() && std::all_of(digits.begin(), digits.end(),
                                                   [](unsigned char c) { return std::isdigit(c); }))
                    vehicles = std::stoi(digits);
            }
        }

        // ---- assemble ---------------------------------------------------------------------
        CVRPProblem problem(depot, customers, capacity, vehicles, name,
                            distanceType == DistanceType::Explicit ? DistanceType::Euclidean : distanceType);
        if (distanceType == DistanceType::Explicit) {
            const auto full = explicitMatrix(labels.size());
            std::vector<std::vector<double>> renumbered(labels.size(), std::vector<double>(labels.size()));
            for (std::size_t i = 0; i < labels.size(); ++i)
                for (std::size_t j = 0; j < labels.size(); ++j)
                    renumbered[i][j] = full[newToOld[i]][newToOld[j]];
            problem.setDistanceMatrix(std::move(renumbered));
        }
        try {
            problem.validate();
        } catch (const std::invalid_argument& e) {
            fail(std::string("invalid instance: ") + e.what());
        }
        return problem;
    }

    std::istream& in_;
    std::string source_;
    std::map<std::string, std::string> header_;
    std::map<std::string, std::size_t> headerLine_;
    std::map<long, std::pair<double, double>> coords_;
    std::map<long, double> demands_;
    std::vector<long> depots_;
    std::vector<double> weights_;
    std::size_t weightsLine_ = 0;
};
}  // namespace

CVRPProblem parseProblem(std::istream& in, const std::string& sourceName) {
    return Parser(in, sourceName).parse();
}

CVRPProblem loadProblem(const std::string& path) {
    std::ifstream file(path);
    if (!file) throw std::runtime_error("cannot open instance file '" + path + "'");
    return parseProblem(file, path);
}
}
