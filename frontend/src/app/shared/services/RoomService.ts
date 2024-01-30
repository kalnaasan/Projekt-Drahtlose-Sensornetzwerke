import {Injectable} from "@angular/core";
import {Observable} from "rxjs";
import {HttpClient} from "@angular/common/http";
import {Room} from "../../dashboard/common/room";
import {environment} from "../../../environments/environment";

@Injectable({
  providedIn: 'root',
})
export class RoomService {
  private apiUrl = environment.SERVER_URL + '/api/v1/web/rooms';

  constructor(private http: HttpClient) {
  }

  getAllRooms(withShape: boolean = false): Observable<Room[]> {
    return this.http.get<Room[]>(`${this.apiUrl}?shape=${withShape}`);
  }

  getRoomById(id: string): Observable<Room> {
    return this.http.get<Room>(`${this.apiUrl}/${id}`);
  }

  createRoom(room: Room): Observable<Room> {
    return this.http.post<Room>(this.apiUrl, room);
  }

  updateRoom(id: string, updatedRoom: Room): Observable<Room> {
    return this.http.put<Room>(`${this.apiUrl}/${id}`, updatedRoom);
  }

  deleteRoom(id: string): Observable<void> {
    return this.http.delete<void>(`${this.apiUrl}/${id}`);
  }


}
