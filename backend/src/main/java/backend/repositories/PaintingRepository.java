package backend.repositories;

import org.springframework.data.jpa.repository.JpaRepository;
import backend.models.Painting;

public interface PaintingRepository extends JpaRepository<Painting, Integer> {
}
