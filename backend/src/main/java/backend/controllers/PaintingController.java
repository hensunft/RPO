package backend.controllers;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.data.domain.Page;
import org.springframework.data.domain.PageRequest;
import org.springframework.data.domain.Sort;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;
import org.springframework.web.server.ResponseStatusException;
import backend.models.Painting;
import backend.repositories.PaintingRepository;

import java.util.List;
import java.util.Optional;

@CrossOrigin(origins = "http://localhost:3000")
@RestController
@RequestMapping("/api/v1/paintings")
public class PaintingController {
    @Autowired
    private PaintingRepository paintingRepository;

    //@GetMapping
    //public List<Painting> getAllPaintings() {
        //return paintingRepository.findAll();
    //}

    @GetMapping()
    public Page<Painting> getAllPaintings(@RequestParam("page") int page, @RequestParam("limit") int limit) {
        return paintingRepository.findAll(PageRequest.of(page, limit, Sort.by(Sort.Direction.ASC, "name")));
    }

    @PostMapping
    public ResponseEntity<Painting> createPainting(@RequestBody Painting painting) {
        try {
            Painting savedPainting = paintingRepository.save(painting);
            return new ResponseEntity<>(savedPainting, HttpStatus.OK);
        } catch (Exception e) {
            return new ResponseEntity<>(null, HttpStatus.BAD_REQUEST);
        }
    }

    @PutMapping("/{id}")
    public ResponseEntity<Painting> updatePainting(@PathVariable int id, @RequestBody Painting painting) {
        Painting paintingObj;
        Optional<Painting> optionalPainting = paintingRepository.findById(id);
        if (optionalPainting.isPresent()) {
            paintingObj = optionalPainting.get();
            paintingObj.setMuseum(painting.getMuseum());
            paintingObj.setArtist(painting.getArtist());
            paintingObj.setYear(painting.getYear());
            paintingObj.setName(painting.getName());
            paintingRepository.save(paintingObj);
            return ResponseEntity.ok(paintingObj);
        } else {
            throw new ResponseStatusException(HttpStatus.NOT_FOUND, "Painting not found");
        }
    }

    @DeleteMapping("/{id}")
    public ResponseEntity<Painting> deletePainting(@PathVariable int id) {
        Optional<Painting> optionalPainting = paintingRepository.findById(id);
        if (optionalPainting.isPresent()) {
            paintingRepository.delete(optionalPainting.get());
        } else throw new ResponseStatusException(HttpStatus.NOT_FOUND, "Painting not found");
        return ResponseEntity.ok(optionalPainting.get());
    }

    @PostMapping("/deletepaintings")
    public ResponseEntity<Painting> delete(@RequestBody List<Painting> paintings) {
        try {
            paintingRepository.deleteAll(paintings);
        } catch (Exception e) {
            System.out.println(e.getMessage());
            throw new ResponseStatusException(HttpStatus.INTERNAL_SERVER_ERROR);
        }
        return ResponseEntity.ok(paintings.get(0));
    }

}


