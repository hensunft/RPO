package backend.controllers;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.data.domain.Page;
import org.springframework.data.domain.PageRequest;
import org.springframework.data.domain.Sort;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.security.crypto.codec.Hex;
import org.springframework.web.bind.annotation.*;
import org.springframework.web.server.ResponseStatusException;
import backend.models.Museum;
import backend.models.User;
import backend.repositories.MuseumRepository;
import backend.repositories.UserRepository;
import backend.tools.DataValidationException;
import backend.tools.Utils;

import java.util.List;
import java.util.Optional;
import java.util.Random;
import java.util.Set;

@CrossOrigin(origins = "http://localhost:3000")
@RestController
@RequestMapping("/api/v1/users")
public class UserController {
    @Autowired
    private UserRepository userRepository;

    @Autowired
    private MuseumRepository museumRepository;

    //@GetMapping
    //public List<User> getAllUsers() {
        //return userRepository.findAll();
    //}

    @GetMapping("/{id}")
    public ResponseEntity<User> getUser(@PathVariable int id) {
        Optional<User> user = userRepository.findById(id);
        if (user.isPresent()) {
            return new ResponseEntity<>(user.get(), HttpStatus.OK);
        }
        return new ResponseEntity<>(HttpStatus.NOT_FOUND);
    }

    @GetMapping()
    public Page<User> getAllUsers(@RequestParam("page") int page, @RequestParam("limit") int limit) {
        return userRepository.findAll(PageRequest.of(page, limit, Sort.by(Sort.Direction.ASC, "login")));
    }

    @PostMapping
    public ResponseEntity<User> createUser(@RequestBody User user) {
        try {
            User savedUser = userRepository.save(user);
            return new ResponseEntity<>(savedUser, HttpStatus.OK);
        } catch (Exception e) {
            return new ResponseEntity<>(null, HttpStatus.BAD_REQUEST);
        }
    }

    @PutMapping("/{id}")
    public ResponseEntity<User> updateUser(@PathVariable int id, @RequestBody User userDetails) throws DataValidationException {
        try {
            User user = userRepository.findById(id)
                    .orElseThrow(() -> new DataValidationException(" Пользователь с таким индексом не найден"));
            user.setEmail(userDetails.getEmail());
            String np = userDetails.getNp();
            if (np != null  && !np.isEmpty()) {
                byte[] b = new byte[32];
                new Random().nextBytes(b);
                String salt = new String(Hex.encode(b));
                user.setPassword(Utils.computeHash(np, salt));
                user.setSalt(salt);
            }
            userRepository.save(user);
            return ResponseEntity.ok(user);
        }
        catch (Exception ex) {
            if (ex.getMessage().contains("users.email_UNIQUE"))
                throw new DataValidationException("Пользователь с такой почтой уже есть в базе");
            else
                throw new DataValidationException("Неизвестная ошибка");
        }
    }

    @DeleteMapping("/{id}")
    public ResponseEntity<User> deleteUser(@PathVariable int id) {
        Optional<User> optionalUser = userRepository.findById(id);
        if (optionalUser.isPresent()) {
            userRepository.delete(optionalUser.get());
            return ResponseEntity.ok(optionalUser.get());
        }
        throw new ResponseStatusException(HttpStatus.NOT_FOUND, "User not found");
    }

    @PostMapping("/{id}/museums")
    public ResponseEntity<User> addMuseums(@PathVariable int id, @RequestBody Set<Museum> museums) {
        Optional<User> optionalUser = userRepository.findById(id);
        if (optionalUser.isPresent()) {
            User user = optionalUser.get();
            for (Museum museum : museums) {
                Optional<Museum> optionalMuseum = museumRepository.findById(museum.getId());
                optionalMuseum.ifPresent(user::addMuseum);
                if (optionalMuseum.isEmpty()) {
                    throw new ResponseStatusException(HttpStatus.NOT_FOUND, "Museum not found");
                }
            }
            userRepository.save(user);
            return ResponseEntity.ok(user);
        } else throw new ResponseStatusException(HttpStatus.NOT_FOUND, "User not found");
    }
    @DeleteMapping("/{id}/museums")
    public ResponseEntity<User> removeMuseums(@PathVariable int id, @RequestBody Set<Museum> museums) {
        Optional<User> optionalUser = userRepository.findById(id);
        if (optionalUser.isPresent()) {
            User user = optionalUser.get();
            for (Museum museum : museums) {
                Optional<Museum> managedMuseum = museumRepository.findById(museum.getId());
                managedMuseum.ifPresent(user::removeMuseum);
            }
            userRepository.save(user);
            return ResponseEntity.ok(user);
        } else {
            throw new ResponseStatusException(HttpStatus.NOT_FOUND, "User not found");
        }
    }

    @PostMapping("/deleteusers")
    public ResponseEntity<User> delete(@RequestBody List<User> users) {
        try {
            userRepository.deleteAll(users);
        } catch (Exception e) {
            System.out.println(e.getMessage());
            throw new ResponseStatusException(HttpStatus.INTERNAL_SERVER_ERROR);
        }
        return ResponseEntity.ok(users.get(0));
    }
}
