package backend.controllers;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.dao.DataIntegrityViolationException;
import org.springframework.data.domain.Page;
import org.springframework.data.domain.PageRequest;
import org.springframework.data.domain.Sort;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;
import org.springframework.web.server.ResponseStatusException;
import backend.models.Artist;
import backend.models.Country;
import backend.repositories.CountryRepository;
import backend.tools.DataValidationException;

import java.util.List;
import java.util.Optional;

@CrossOrigin(origins = "http://localhost:3000")
@RestController
@RequestMapping("/api/v1/countries")
public class CountryController {
    @Autowired
    CountryRepository countryRepository;

    @GetMapping("/old")
    public List<Country> getAllCountries() {
        return countryRepository.findAll(Sort.by(Sort.Direction.ASC, "name"));
    }

    @GetMapping()
    public Page<Country> getAllCountries(@RequestParam("page") int page, @RequestParam("limit") int limit) {
        return countryRepository.findAll(PageRequest.of(page, limit, Sort.by(Sort.Direction.ASC, "name")));
    }

    @GetMapping("/{id}")
    public ResponseEntity<Country> getCountryById(@PathVariable Long id) throws DataValidationException {
        System.out.println("getCountryById");
        Country country = countryRepository.findById(id).orElseThrow(() -> new DataValidationException("No such country"));
        return ResponseEntity.ok(country);
    }

    @PostMapping
    public ResponseEntity<Country> createCountry(@RequestBody Country country) {
        System.out.println("createCountry");
        try {
            if (country.id == -1){
                Country saveableCountry = new Country();
                saveableCountry.name = country.name;
                saveableCountry.artists = country.artists;
                countryRepository.save(saveableCountry);
                return new ResponseEntity<>(saveableCountry, HttpStatus.OK);
            }
            Country savedCountry = countryRepository.save(country);
            return new ResponseEntity<>(savedCountry, HttpStatus.OK);
        } catch (Exception e) {
            System.out.println(e.getMessage());
            if (e instanceof DataIntegrityViolationException) {
                return new ResponseEntity<>(HttpStatus.CONFLICT);
            }
            return new ResponseEntity<>(null, HttpStatus.INTERNAL_SERVER_ERROR);
        }
    }

    @PutMapping("/{id}")
    public ResponseEntity<Country> updateCountry(@RequestBody Country country, @PathVariable long id) {
        Country countryObject;
        System.out.println("updateCountry");
        Optional<Country> cc = countryRepository.findById(id);
        if (cc.isPresent()) {
            try {
                countryObject = cc.get();
                countryObject.name = country.name;
                countryRepository.save(countryObject);
                return ResponseEntity.ok(countryObject);
            } catch (Exception e) {
                if (e instanceof DataIntegrityViolationException) {
                    return new ResponseEntity<>(HttpStatus.CONFLICT);
                }
                return new ResponseEntity<>(null, HttpStatus.INTERNAL_SERVER_ERROR);
            }
        } else {
            throw new ResponseStatusException(HttpStatus.NOT_FOUND, "Country not found");
        }
    }

    @DeleteMapping("/{id}")
    public ResponseEntity<Country> deleteCountry(@PathVariable Long id) {
        Optional<Country> country = countryRepository.findById(id);
        System.out.println("deleteCountry");
        if (country.isPresent()) {
            countryRepository.delete(country.get());
        }
        else throw new ResponseStatusException(HttpStatus.NOT_FOUND, "Artist not found");
        return ResponseEntity.ok(country.get());
    }

    @PostMapping("/deletecountries")
    public ResponseEntity<Country> deleteCountries(@RequestBody List<Country> countries) {
        System.out.println("deleteCountries");
        try {
            countryRepository.deleteAll(countries);
        } catch (Exception e) {
            System.out.println(e.getMessage());
            throw new ResponseStatusException(HttpStatus.INTERNAL_SERVER_ERROR);
        }
        return ResponseEntity.ok(countries.get(0));
    }

    @GetMapping("/{id}/artists")
    public ResponseEntity<List<Artist>> getArtistsByCountry(@PathVariable long id) {
        Optional<Country> country = countryRepository.findById(id);
        System.out.println("getArtistsByCountry");
        if (country.isPresent()) {
            return ResponseEntity.ok(country.get().artists);
        }
        else throw new ResponseStatusException(HttpStatus.NOT_FOUND, "Country not found");
    }
}
