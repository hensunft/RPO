package backend.controllers;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.data.domain.Page;
import org.springframework.data.domain.PageRequest;
import org.springframework.data.domain.Sort;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;
import org.springframework.web.server.ResponseStatusException;
import backend.models.Artist;
import backend.repositories.ArtistRepository;

import java.util.List;
import java.util.Optional;
@CrossOrigin(origins = "http://localhost:3000")
@RestController
@RequestMapping("/api/v1/artists")
public class ArtistController {
    @Autowired
    private ArtistRepository artistRepository;

    //@GetMapping
    //public List<Artist> getAllArtists() {
        //return artistRepository.findAll();
    //}

    @GetMapping()
    public Page<Artist> getAllArtists(@RequestParam("page") int page, @RequestParam("limit") int limit) {
        return artistRepository.findAll(PageRequest.of(page, limit, Sort.by(Sort.Direction.ASC, "name")));
    }

    @PostMapping
    public ResponseEntity<Artist> addArtist(@RequestBody Artist artist) {
        try{
            Artist savedArtist = artistRepository.save(artist);
            return new ResponseEntity<>(savedArtist, HttpStatus.OK);
        } catch (Exception e) {
            return new ResponseEntity<>(null, HttpStatus.BAD_REQUEST);
        }
    }

    @PutMapping("/{id}")
    public ResponseEntity<Artist> updateArtist(@RequestBody Artist artist, @PathVariable int id) {
        Artist artistObj;
        Optional<Artist> optionalArtist = artistRepository.findById(id);
        if (optionalArtist.isPresent()) {
            artistObj = optionalArtist.get();
            artistObj.setName(artist.getName());
            artistObj.setAge(artist.getAge());
            artistObj.setCountry(artist.getCountry());
            artistRepository.save(artistObj);
            return ResponseEntity.ok(artistObj);
        } else {
            throw new ResponseStatusException(HttpStatus.NOT_FOUND, "Artist not found");
        }
    }

    @DeleteMapping("/{id}")
    public ResponseEntity<Artist> deleteArtist(@PathVariable int id) {
        Optional<Artist> optionalArtist = artistRepository.findById(id);
        if (optionalArtist.isPresent()) {
            artistRepository.delete(optionalArtist.get());
        } else {
            throw new ResponseStatusException(HttpStatus.NOT_FOUND, "Artist not found");
        }
        return ResponseEntity.ok(optionalArtist.get());
    }

    @PostMapping("/deleteartists")
    public ResponseEntity<Artist> delete(@RequestBody List<Artist> artists) {
        try {
            artistRepository.deleteAll(artists);
        } catch (Exception e) {
            System.out.println(e.getMessage());
            throw new ResponseStatusException(HttpStatus.INTERNAL_SERVER_ERROR);
        }
        return ResponseEntity.ok(artists.get(0));
    }
}
