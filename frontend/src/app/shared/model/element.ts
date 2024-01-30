import {Shape} from './shape';
import {Room} from '../../dashboard/common/room';

export interface Element {
  id: string;
  type: string;
  start: string;
  end: string;
  shape: Shape | null;
  room: Room | null;
  createdAt: Date;
  updatedAt: Date;
}
