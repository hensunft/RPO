package backend.repositories;

import org.springframework.data.jpa.repository.JpaRepository;
import backend.models.Artist;

public interface ArtistRepository extends JpaRepository<Artist, Integer> {}
