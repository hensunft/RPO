package backend.controllers;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.data.domain.Page;
import org.springframework.data.domain.PageRequest;
import org.springframework.data.domain.Sort;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;
import org.springframework.web.server.ResponseStatusException;
import backend.models.Museum;
import backend.repositories.MuseumRepository;

import java.util.List;
import java.util.Optional;
@CrossOrigin(origins = "http://localhost:3000")
@RestController
@RequestMapping("/api/v1/museums")
public class MuseumController {
    @Autowired
    private MuseumRepository museumRepository;

    //@GetMapping
    //public List<Museum> getAllMuseums() {
        //return museumRepository.findAll();
    //}

    @GetMapping()
    public Page<Museum> getAllMuseums(@RequestParam("page") int page, @RequestParam("limit") int limit) {
        return museumRepository.findAll(PageRequest.of(page, limit, Sort.by(Sort.Direction.ASC, "name")));
    }

    @PostMapping
    public ResponseEntity<Museum> createMuseum(@RequestBody Museum museum) {
        try{
            if (museum.getId() == -1) {
                Museum saveable = new Museum();
                saveable.setName(museum.getName());
                saveable.setLocation( museum.getLocation());
                saveable.setPaintings(museum.getPaintings());
                saveable.setUsers(museum.getUsers());
                museumRepository.saveAndFlush(saveable);
                return new ResponseEntity<>(saveable, HttpStatus.OK);
            }
            Museum createdMuseum = museumRepository.save(museum);
            return new ResponseEntity<>(createdMuseum, HttpStatus.OK);
        } catch (Exception e) {
            return new ResponseEntity<>(null, HttpStatus.BAD_REQUEST);
        }
    }

    @PutMapping("/{id}")
    public ResponseEntity<Museum> updateMuseum(@RequestBody Museum museum, @PathVariable int id) {
        Museum museumObj;
        Optional<Museum> optionalMuseum = museumRepository.findById(id);
        if (optionalMuseum.isPresent()) {
            museumObj = optionalMuseum.get();
            museumObj.setName(museum.getName());
            museumObj.setLocation(museum.getLocation());
            museumRepository.save(museumObj);
            return ResponseEntity.ok(museumObj);
        } else {
            throw new ResponseStatusException(HttpStatus.NOT_FOUND, "Museum not found");
        }
    }

    @DeleteMapping("/{id}")
    public ResponseEntity<Museum> deleteMuseum(@PathVariable int id) {
        Optional<Museum> optionalMuseum = museumRepository.findById(id);
        if (optionalMuseum.isPresent()) {
            museumRepository.delete(optionalMuseum.get());
        } else throw new ResponseStatusException(HttpStatus.NOT_FOUND, "Museum not found");
        return ResponseEntity.ok(optionalMuseum.get());
    }
    @PostMapping("/deletemuseums")
    public ResponseEntity<Museum> delete(@RequestBody List<Museum> museums) {
        try {
            museumRepository.deleteAll(museums);
        } catch (Exception e) {
            System.out.println(e.getMessage());
            throw new ResponseStatusException(HttpStatus.INTERNAL_SERVER_ERROR);
        }
        return ResponseEntity.ok(museums.get(0));
    }
}
