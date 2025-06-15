package backend.repositories;

import org.springframework.data.jpa.repository.JpaRepository;
import backend.models.Museum;

public interface MuseumRepository extends JpaRepository<Museum, Integer> {
}
